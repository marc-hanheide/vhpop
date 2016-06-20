/*
 * Copyright (C) 2002-2004 Carnegie Mellon University
 * Written by H�kan L. S. Younes.
 *
 * Permission is hereby granted to distribute this software for
 * non-commercial research purposes, provided that this copyright
 * notice is included with any such distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 * SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *
 * $Id: actions.cc,v 6.7 2004-02-07 11:33:54 lorens Exp $
 */
#include "actions.h"
#include "bindings.h"
#include "problems.h"
#include <limits>
#include <stack>
#include <typeinfo>
#include <memory>


/* ====================================================================== */
/* Action */

/* Next action id. */
size_t Action::next_id = 0;


/* Constructs an action with the given name. */
Action::Action(const std::string& name, bool durative)
  : id_(next_id++), name_(name), condition_(&Formula::TRUE),
    durative_(durative), min_duration_(new Value(0.0f)),
    max_duration_(new Value(durative ?
                            std::numeric_limits<float>::infinity() : 0.0f)) {
  Formula::register_use(condition_);
  RCObject::ref(min_duration_);
  RCObject::ref(max_duration_);
}

/*copy constructor*/
Action::Action(const Action &s)
    : id_(s.id_), name_(s.name_), durative_(s.durative_)
{

    //copy effects
    this->effects_ = EffectList();//std::vector<const Effect *>();
    for(int i=0; i< s.effects_.size();i++)
    {
       this->effects_.push_back(new Effect(*s.effects_.at(i)));
    }
    if (s.condition_ !=0)
    {
        this->condition_ = s.condition_->clone();
        Formula::register_use(condition_);
    }
    else
        this->condition_ = 0;

    if (s.min_duration_ != 0)
    {
        this->min_duration_ = s.min_duration_->clone();
        RCObject::ref(min_duration_);
    }
    else
        this->min_duration_ = 0;

    if (s.max_duration_ != 0)
    {
        this->max_duration_ = s.max_duration_->clone();
        RCObject::ref(max_duration_);
    }
    else
        this->max_duration_ = 0;

}

bool operator== (Action &a1, Action &a2)
{

    if(a1.id_ != a2.id_)
        return false;
    if(a1.name_ != a2.name_)
        return false;
    if(a1.durative_ != a2.durative_)
        return false;
    if(a1.min_duration_->value()!=a2.min_duration_->value())
        return false;
    if(a1.max_duration_->value()!=a2.max_duration_->value())
        return false;

    //TODO this comparision is missing effects
    int iter = 0;

    FormulaList formList;
    formList.push_back(a1.condition_->clone());

    FormulaList formList2;
    formList2.push_back(a2.condition_->clone());

    FormulaList::iterator itf = formList.begin();
    FormulaList::iterator itf2 = formList2.begin();



    while(itf < formList.end())
    {
        char typeOfFormula = (*itf)->type();
        char typeOfFormula2 = (*itf2)->type();
        if(typeOfFormula != typeOfFormula2)
        {
            return false;
        }
        switch(typeOfFormula)
        {
           case 'a':
           {
             std::unique_ptr<const Atom> at (dynamic_cast<const Atom*>(*itf));
             std::unique_ptr<const Atom> at2 (dynamic_cast<const Atom*>(*itf2));
             Atom a1 = *at;
             Atom a2 = *at2;
             if (!(a1 == a2))
             {
                 return false;
             }
             break;
           }
           case 'n':
           {
             std::unique_ptr<const Negation> neg (dynamic_cast<const Negation*>(*itf));
             std::unique_ptr<const Negation> neg2 (dynamic_cast<const Negation*>(*itf2));
             Negation n1 = *neg;
             Negation n2 = *neg2;
             if(!(n1 == n2))
             {
                 return false;
             }
             break;
           }
           case 'j':
           {
             std::unique_ptr<const Conjunction> conj (dynamic_cast<const Conjunction*>(*itf));
             std::unique_ptr<const Conjunction> conj2 (dynamic_cast<const Conjunction*>(*itf2));
             FormulaList tempL = conj->conjuncts();
             FormulaList tempL2 = conj2->conjuncts();

             if(tempL.size() != tempL2.size())
                 return false;

             for(int i = 0;i<tempL.size();i++)
             {
                 //need to create a new pointer
               formList.push_back( tempL.at(i)->clone());
               formList2.push_back( tempL2.at(i)->clone());
             }
             break;
           }
           case 't':
           {
             std::unique_ptr<const TimedLiteral> tl (dynamic_cast<const TimedLiteral*>(*itf));
             std::unique_ptr<const TimedLiteral> tl2 (dynamic_cast<const TimedLiteral*>(*itf2));
             formList.push_back(tl->literal().clone());
             formList2.push_back(tl2->literal().clone());
             break;
           }
           default:
           {
            std::cout << "this type of formula is not yet covered\n";
            throw;
            break;
           }
        }
        iter++;
        itf = formList.begin()+iter;//increase iterator
        itf2 = formList2.begin()+iter;//increase iterator
     }

     return true;
}

/* Deletes this action. */
Action::~Action() {
  Formula::unregister_use(condition_);
  RCObject::destructive_deref(min_duration_);
  RCObject::destructive_deref(max_duration_);
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    delete *ei;
  }
}


/* Sets the condition for this action. */
void Action::set_condition(const Formula& condition) {
  if (condition_ != &condition) {
    Formula::register_use(&condition);
    Formula::unregister_use(condition_);
    condition_ = &condition;
  }
}


/* Adds an effect to this action. */
void Action::add_effect(const Effect& effect) {
  effects_.push_back(&effect);
}


/* Sets the minimum duration for this action. */
void Action::set_min_duration(const Expression& min_duration) {
  const Expression& md = Maximum::make(*min_duration_, min_duration);
  if (&md != min_duration_) {
    RCObject::ref(&md);
    RCObject::destructive_deref(min_duration_);
    min_duration_ = &md;
  }
}


/* Sets the maximum duration for this action. */
void Action::set_max_duration(const Expression& max_duration) {
  const Expression& md = Minimum::make(*max_duration_, max_duration);
  if (&md != max_duration_) {
    RCObject::ref(&md);
    RCObject::destructive_deref(max_duration_);
    max_duration_ = &md;
  }
}


/* Sets the duration for this action. */
void Action::set_duration(const Expression& duration) {
  set_min_duration(duration);
  set_max_duration(duration);
}


/* "Strengthens" the effects of this action. */
void Action::strengthen_effects(const Domain& domain) {
  /*
   * Separate negative effects from positive effects occuring at the
   * same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    if (typeid(ei.literal()) == typeid(Negation)) {
      const Negation& neg = dynamic_cast<const Negation&>(ei.literal());
      const Formula* cond = &Formula::TRUE;
      for (EffectList::const_iterator j = effects_.begin();
	   j != effects_.end() && !cond->contradiction(); j++) {
	const Effect& ej = **j;
	if (ei.when() == ej.when() && typeid(ej.literal()) == typeid(Atom)) {
	  bool diff_param = ei.arity() != ej.arity();
	  for (size_t pi = 0; pi < ei.arity() && !diff_param; pi++) {
	    if (ei.parameter(pi) != ej.parameter(pi)) {
	      diff_param = true;
	    }
	  }
	  if (!diff_param) {
	    /* Only separate two effects with same universally
	       quantified variables. */
	    BindingList mgu;
	    if (Bindings::unifiable(mgu, neg.atom(), 1, ej.literal(), 1)) {
	      const Formula* sep = &Formula::FALSE;
	      for (BindingList::const_iterator si = mgu.begin();
		   si != mgu.end(); si++) {
		const Binding& subst = *si;
		sep = &(*sep || Inequality::make(subst.var(), subst.term()));
	      }
	      cond = &(*cond && (*sep || !ej.condition()));
	    }
	  }
	}
      }
      if (!cond->tautology()) {
	ei.set_link_condition(*cond);
      }
    }
  }

  /*
   * Separate effects from conditions asserted at the same time.
   */
  for (size_t i = 0; i < effects_.size(); i++) {
    const Effect& ei = *effects_[i];
    ei.set_link_condition(ei.link_condition()
			  && condition().separator(ei, domain));
  }
}


/* ====================================================================== */
/* ActionSchema */

/* Constructs an action schema with the given name. */
ActionSchema::ActionSchema(const std::string& name, bool durative)
  : Action(name, durative) {}


/*copy constructor*/
ActionSchema::ActionSchema(const ActionSchema& s)
    : Action(s),parameters_(s.parameters_){}

/* Adds a parameter to this action schema. */
void ActionSchema::add_parameter(Variable var) {
  parameters_.push_back(var);
}


/* Fills the provided action list with all instantiations of this
   action schema. */
void ActionSchema::instantiations(GroundActionList& actions,
				  const Problem& problem) const {
  size_t n = parameters().size();
  if (n == 0) {
    const GroundAction* inst_action =
      instantiation(SubstitutionMap(), problem, condition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    SubstitutionMap args;
    std::vector<const ObjectList*> arguments(n);
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++)
    {
      const Type& t = TermTable::type(parameters()[i]);
      arguments[i] = &problem.terms().compatible_objects(t);
      if (arguments[i]->empty())
      {
        return;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    std::stack<const Formula*> conds;
    conds.push(&condition());
    Formula::register_use(conds.top());
    for (size_t i = 0; i < n; )
    {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Formula& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Formula::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction())
      {
          if (!inst_cond.contradiction())
          {
            const GroundAction* inst_action = instantiation(args, problem, inst_cond);
            if (inst_action != NULL)
            {
               actions.push_back(inst_action);
            }
          }
          for (int j = i; j >= 0; j--)
          {
            Formula::unregister_use(conds.top());
            conds.pop();
            args.erase(parameters()[j]);
            next_arg[j]++;
            if (next_arg[j] == arguments[j]->end())
            {
              if (j == 0)
              {
                i = n;
                break;
              }
              else
              {
                next_arg[j] = arguments[j]->begin();
              }
            }
            else
            {
              i = j;
              break;
            }
         }
    }
    else
    {
      i++;
    }
  }
  while (!conds.empty())
  {
      Formula::unregister_use(conds.top());
      conds.pop();
  }
 }
}

void ActionSchema::groundToSpecific(GroundActionList& actions,
                  const Problem& problem, std::vector<const ObjectList*> arguments) const {
  size_t n = parameters().size();
  if (n == 0) {
    const GroundAction* inst_action =
      instantiation(SubstitutionMap(), problem, condition());
    if (inst_action != NULL) {
      actions.push_back(inst_action);
    }
  } else {
    SubstitutionMap args;
    //std::vector<const ObjectList*> arguments(n);
    std::vector<ObjectList::const_iterator> next_arg;
    for (size_t i = 0; i < n; i++)
    {
      const Type& t = TermTable::type(parameters()[i]);
      //arguments[i] = &problem.terms().compatible_objects(t);
      if (arguments[i]->empty())
      {
        return;
      }
      next_arg.push_back(arguments[i]->begin());
    }
    std::stack<const Formula*> conds;
    conds.push(&condition());
    Formula::register_use(conds.top());
    for (size_t i = 0; i < n; )
    {
      args.insert(std::make_pair(parameters()[i], *next_arg[i]));
      SubstitutionMap pargs;
      pargs.insert(std::make_pair(parameters()[i], *next_arg[i]));
      const Formula& inst_cond = conds.top()->instantiation(pargs, problem);
      conds.push(&inst_cond);
      Formula::register_use(conds.top());
      if (i + 1 == n || inst_cond.contradiction())
      {
          if (!inst_cond.contradiction())
          {
            const GroundAction* inst_action = instantiation(args, problem, inst_cond);
            if (inst_action != NULL)
            {
               actions.push_back(inst_action);
            }
          }
          for (int j = i; j >= 0; j--)
          {
            Formula::unregister_use(conds.top());
            conds.pop();
            args.erase(parameters()[j]);
            next_arg[j]++;
            if (next_arg[j] == arguments[j]->end())
            {
              if (j == 0)
              {
                i = n;
                break;
              }
              else
              {
                next_arg[j] = arguments[j]->begin();
              }
            }
            else
            {
              i = j;
              break;
            }
         }
    }
    else
    {
      i++;
    }
  }
  while (!conds.empty())
  {
      Formula::unregister_use(conds.top());
      conds.pop();
  }
 }
}


/* Returns an instantiation of this action schema. */
const GroundAction*
ActionSchema::instantiation(const SubstitutionMap& args,
			    const Problem& problem,
			    const Formula& condition) const {
  EffectList inst_effects;
  size_t useful = 0;
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    (*ei)->instantiations(inst_effects, useful, args, problem);
  }
  if (useful > 0) {
    GroundAction& ga = *new GroundAction(name(), durative());
    size_t n = parameters().size();
    for (size_t i = 0; i < n; i++) {
      SubstitutionMap::const_iterator si = args.find(parameters()[i]);
      ga.add_argument((*si).second.as_object());
    }
    ga.set_condition(condition);
    for (EffectList::const_iterator ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      ga.add_effect(**ei);
    }
    ga.set_min_duration(min_duration().instantiation(args,
						     problem.init_values()));
    ga.set_max_duration(max_duration().instantiation(args,
						     problem.init_values()));
    const Value* v1 = dynamic_cast<const Value*>(&ga.min_duration());
    if (v1 != NULL) {
      const Value* v2 = dynamic_cast<const Value*>(&ga.max_duration());
      if (v2 != NULL) {
	if (v1->value() > v2->value()) {
	  delete &ga;
	  return NULL;
	}
      }
    }
    return &ga;
  } else {
    for (EffectList::const_iterator ei = inst_effects.begin();
	 ei != inst_effects.end(); ei++) {
      delete *ei;
    }
    return NULL;
  }
}


/* Prints this action on the given stream. */
void ActionSchema::print(std::ostream& os) const {
  os << "  " << name();
  os << std::endl << "    parameters:";
  for (VariableList::const_iterator vi = parameters().begin();
       vi != parameters().end(); vi++) {
    os << ' ' << *vi;
  }
  if (durative()) {
    os << std::endl << "    duration: [" << min_duration() << ','
       << max_duration() << "]";
  }
  os << std::endl << "    condition: ";
  condition().print(os, 0, Bindings::EMPTY);
  os << std::endl << "    effect: (and";
  for (EffectList::const_iterator ei = effects().begin();
       ei != effects().end(); ei++) {
    os << ' ';
    (*ei)->print(os);
  }
  os << ")";
}


/* Prints this action on the given stream with the given bindings. */
void ActionSchema::print(std::ostream& os,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (VariableList::const_iterator ti = parameters().begin();
       ti != parameters().end(); ti++) {
    os << ' ';
    bindings.print_term(os, *ti, step_id);
  }
  os << ')';
}


/* ====================================================================== */
/* GroundAction */

/* Constructs a ground action with the given name. */
GroundAction::GroundAction(const std::string& name, bool durative)
  : Action(name, durative) {}

/*copy constructor*/
GroundAction::GroundAction(const GroundAction &s)
    : Action(s),arguments_(s.arguments_) {}

/*constructor used in the parsePlan*/
GroundAction::GroundAction(const ActionSchema &s)
    : Action(s),arguments_(ObjectList())
{
}

/* Adds an argument to this ground action. */
void GroundAction::add_argument(Object arg) {
  arguments_.push_back(arg);
  //when creating the ground action, I need to use it as the bindings
  //Binding(const Variable& var, size_t var_id, const Term& term, size_t term_id,bool equality)
}


/* Prints this action on the given stream  */
void GroundAction::print(std::ostream& os) const {
  os << '(' << name();
  for (ObjectList::const_iterator ni = arguments().begin();
       ni != arguments().end(); ni++) {
    os << ' ' << *ni;
  }
  os << ')';
}

/* Prints this action on the given stream with the given bindings. */
void GroundAction::print(std::ostream& os,
			 size_t step_id, const Bindings& bindings) const {
  os << '(' << name();
  for (ObjectList::const_iterator ni = arguments().begin();
       ni != arguments().end(); ni++) {
    os << ' ' << *ni;
  }
  os << ')';
}

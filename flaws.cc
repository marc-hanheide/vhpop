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
 * $Id: flaws.cc,v 6.2 2003-07-21 02:17:35 lorens Exp $
 */
#include "flaws.h"
#include "plans.h"
#include "domains.h"
#include "formulas.h"
#include "effects.h"


/* ====================================================================== */
/* OpenCondition */

/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Formula& condition)
  : step_id_(step_id), condition_(&condition) {
  Formula::register_use(condition_);
}


/* Constructs an open condition. */
OpenCondition::OpenCondition(size_t step_id, const Literal& condition,
			     FormulaTime when)
  : step_id_(step_id), condition_(&condition), when_(when) {
  Formula::register_use(condition_);
}


/* Constructs an open condition. */
OpenCondition::OpenCondition(const OpenCondition& oc)
  : step_id_(oc.step_id_), when_(oc.when_) {

    if(oc.condition_ != 0)
        this->condition_ = oc.condition_->clone();
    else
        this->condition_ = 0;

  Formula::register_use(condition_);
}


/* Deletes this open condition. */
OpenCondition::~OpenCondition() {
  Formula::unregister_use(condition_);
}


/* Checks if this is a static open condition. */
bool OpenCondition::is_static() const {
  const Literal* lit = literal();
  return (lit != NULL && step_id() != Plan::GOAL_ID
	  && PredicateTable::static_predicate(lit->predicate()));
}


/* Returns a literal, or NULL if this is not a literal open
   condition. */
const Literal* OpenCondition::literal() const {
  return dynamic_cast<const Literal*>(condition_);
}


/* Returns a inequality, or NULL if this is not an inequality open
   condition. */
const Inequality* OpenCondition::inequality() const {
  return dynamic_cast<const Inequality*>(condition_);
}


/* Returns a disjunction, or NULL if this is not a disjunctive open
   condition. */
const Disjunction* OpenCondition::disjunction() const {
  return dynamic_cast<const Disjunction*>(condition_);
}


/* Prints this object on the given stream. */
void OpenCondition::print(std::ostream& os, const Bindings& bindings) const {
  os << "#<OPEN ";
  condition().print(os, step_id(), bindings);
  os << ' ' << step_id() << ">";
}


/* ====================================================================== */
/* Unsafe */

/* Prints this object on the given stream. */
void Unsafe::print(std::ostream& os, const Bindings& bindings) const {
  os << "#<UNSAFE " << link().from_id() << ' ';
  link().condition().print(os, link().to_id(), bindings);
  os << ' ' << link().to_id() << " step " << step_id() << ">";
}

Unsafe::Unsafe(const Unsafe &o)
    :step_id_(o.step_id_)
{
    if(o.link_ != 0)
        this->link_ = new Link(*o.link_);
    else
        this->link_ = 0;


    if(o.effect_ != 0)
        this->effect_ = new Effect(*o.effect_);
    else
        this->effect_ = 0;
}


/* ====================================================================== */
/* MutexThreat */

  /*Lenka fixixng the empty constructor*/
  MutexThreat::MutexThreat()
  {
    this->step_id1_ = 0;
    this->step_id2_ = 0;
    this->effect1_ = new Effect();
    this->effect2_ = new Effect();
  }

  MutexThreat::MutexThreat(const MutexThreat& o)
     :step_id1_(o.step_id1_), step_id2_(o.step_id2_)
  {
      //std::cout <<"in mutexthreat copy constructor\n";
      if(o.effect1_ != 0)
          this->effect1_ = new Effect(*o.effect1_);
      else
          this->effect1_ = 0;

      if(o.effect2_ != 0)
          this->effect2_ = new Effect(*o.effect2_);
      else
          this->effect2_ = 0;

  }

/* Prints this object on the given stream. */
void MutexThreat::print(std::ostream& os, const Bindings& bindings) const {
  os << "#<MUTEX " << step_id1() << " ";
  //TODO fix this
  //effect1().literal().print(os, step_id1(), bindings);
  os << ' ' << step_id2() << ' ';
  //effect2().literal().print(os, step_id2(), bindings);
  os << '>';
}

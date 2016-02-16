/* -*-C++-*- */
/*
 * PDDL expressions.
 *
 * Copyright (C) 2002-2005 Carnegie Mellon University
 *
 * This file is part of VHPOP.
 *
 * VHPOP is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VHPOP is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VHPOP; if not, write to the Free Software Foundation,
 * Inc., #59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Id: expressions.h,v 6.1 2003-12-05 23:19:02 lorens Exp $
 */
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "config.h"
#include "refcount.h"
#include "functions.h"
#include "terms.h"
#include <iostream>
#include <map>
#include <set>


/* ====================================================================== */
/* Expression. */

struct ValueMap;

/*
 * An abstract expression.
 */
struct Expression : public RCObject {
  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const = 0;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const = 0;

  /*copy formula*/
  Expression(const Expression &s)
   :RCObject(s)
  {
  }

  Expression()
    :RCObject()
  {
  }

  virtual const Expression * clone () const = 0;

  virtual float value() const = 0;

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, const Expression& e);
};

/* Output operator for expressions. */
std::ostream& operator<<(std::ostream& os, const Expression& e);


/* ====================================================================== */
/* Value */

/*
 * A constant value.
 */
struct Value : public Expression {
  /* Constructs a constant value. */
  explicit Value(float value) : value_(value) {}

  /* Returns the value of this expression. */
  float value() const { return value_; }

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Value& instantiation(const SubstitutionMap& subst,
				     const ValueMap& values) const;

  /*copy formula*/
  Value(const Value &s)
      :Expression(s), value_(s.value_)
  {
  }

  const Value * clone () const
  {
     return new Value(*this);
   }

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* The value. */
  float value_;
};


/* ====================================================================== */
/* Fluent */

/*
 * A fluent.
 */
struct Fluent : public Expression {
  /* Returns a fluent with the given function and terms. */
  static const Fluent& make(const Function& function, const TermList& terms);

  /* Deletes this fluent. */
  virtual ~Fluent();

  /* Returns the id for this fluent (zero if lifted). */
  size_t id() const { return id_; }

  /* Returns the function of this fluent. */
  const Function& function() const { return function_; }

  /* Returns the terms of this fluent. */
  const TermList& terms() const { return terms_; }

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns this fluent subject to the given substitution. */
  const Fluent& substitution(const SubstitutionMap& subst) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;

  //TODO maybe issue with not proper copying of fluents?
  Fluent(const Fluent &s)
      :Expression(s), id_(s.id_), function_(s.function_), terms_(s.terms_)
  {
      this->next_id = s.next_id;
      this->fluents = s.fluents;
  }

  const Fluent * clone () const
  {
     return new Fluent(*this);
   }

  float value() const { return -1.0; }

protected:
  /* Assigns an id to this fluent. */
  void assign_id(bool ground);

  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Less-than comparison function object for fluents. */
  struct FluentLess
    : public std::binary_function<const Fluent*, const Fluent*, bool> {
    /* Comparison function. */
    bool operator()(const Fluent* f1, const Fluent* f2) const;
  };

  /* A table of fluents. */
  struct FluentTable : std::set<const Fluent*, FluentLess> {
  };

  /* Table of fluents. */
  static FluentTable fluents;
  /* Next id for ground fluents. */
  static size_t next_id;

  /* Unique id for ground fluents (zero if lifted). */
  size_t id_;
  /* Function of this fluent. */
  Function function_;
  /* Terms of this fluent. */
  TermList terms_;

  /* Constructs a fluent with the given function. */
  explicit Fluent(const Function& function) : function_(function) {}

  /* Adds a term to this fluent. */
  void add_term(Term term) { terms_.push_back(term); }
};

/*
 * Less than function object for fluent pointers.
 */
namespace std {
  template<>
  struct less<const Fluent*>
    : public binary_function<const Fluent*, const Fluent*, bool> {
    /* Comparison function operator. */
    bool operator()(const Fluent* f1, const Fluent* f2) const {
      return f1->id() < f2->id();
    }
  };
}


/* ====================================================================== */
/* Computation */

/*
 * A computation expression.
 */
struct Computation : public Expression {
  /* Deletes this computation. */
  virtual ~Computation();

  /* Returns the first operand for this computation. */
  const Expression& operand1() const { return *operand1_; }

  /* Returns the second operand for this computation. */
  const Expression& operand2() const { return *operand2_; }

  Computation(const Computation &s)
      :Expression(s)
  {
      if (s.operand1_ !=0)
          this->operand1_ = s.operand1_->clone();
      else
          this->operand1_ = 0;

      if(s.operand2_ != 0)
          this->operand2_ = s.operand2_->clone();
      else
          this->operand2_ = 0;

      ref(operand1_);
      ref(operand2_);
  }

  virtual const Computation * clone () const
  {
   }

  virtual float value() const {}

protected:
  /* Constructs a computation. */
  Computation(const Expression& operand1, const Expression& operand2);

private:
  /* The first operand for this computation. */
  const Expression* operand1_;
  /* The second operand for this computation. */
  const Expression* operand2_;
};


/* ====================================================================== */
/* Addition */

/*
 * An addition.
 */
struct Addition : public Computation {
  /* Returns an addition of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;

  Addition(const Addition &s)
      :Computation(s)
  {
  }

  const Addition * clone () const
  {
     return new Addition(*this);
   }

   float value() const { return -1.0; }

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs an addition. */
  Addition(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Subtraction */

/*
 * A subtraction.
 */
struct Subtraction : public Computation {
  /* Returns a subtraction of the two expressions. */
  static const Expression& make(const Expression& term1,
				const Expression& term2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;


  Subtraction(const Subtraction &s)
      :Computation(s)
  {
  }

  const Subtraction * clone () const
  {
     return new Subtraction(*this);
   }


protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a subtraction. */
  Subtraction(const Expression& term1, const Expression& term2)
    : Computation(term1, term2) {}
};


/* ====================================================================== */
/* Multiplication */

/*
 * A multiplication.
 */
struct Multiplication : public Computation {
  /* Returns a multiplication of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;


  Multiplication(const Multiplication &s)
      :Computation(s)
  {
  }

  const Multiplication * clone () const
  {
     return new Multiplication(*this);
   }




protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a multiplication. */
  Multiplication(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Division */

/*
 * A division.
 */
struct Division : public Computation {
  /* Returns a division of the two expressions. */
  static const Expression& make(const Expression& factor1,
				const Expression& factor2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;

  Division(const Division &s)
      :Computation(s)
  {
  }

  const Division * clone () const
  {
     return new Division(*this);
   }

protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a division. */
  Division(const Expression& factor1, const Expression& factor2)
    : Computation(factor1, factor2) {}
};


/* ====================================================================== */
/* Minimum */

/*
 * The minimum of two expressions.
 */
struct Minimum : public Computation {
  /* Returns the minimum of the two expressions. */
  static const Expression& make(const Expression& operand1,
				const Expression& operand2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;

  Minimum(const Minimum &s)
      :Computation(s)
  {
  }

  const Minimum * clone () const
  {
     return new Minimum(*this);
   }



protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a minimum. */
  Minimum(const Expression& operand1, const Expression& operand2)
    : Computation(operand1, operand2) {}
};


/* ====================================================================== */
/* Maximum */

/*
 * The maximum of two expressions.
 */
struct Maximum : public Computation {
  /* Returns the maximum of the two expressions. */
  static const Expression& make(const Expression& operand1,
				const Expression& operand2);

  /* Returns the value of this expression in the given state. */
  virtual float value(const ValueMap& values) const;

  /* Returns an instantiation of this expression. */
  virtual const Expression& instantiation(const SubstitutionMap& subst,
					  const ValueMap& values) const;


  Maximum(const Maximum &s)
      :Computation(s)
  {
  }

  const Maximum * clone () const
  {
     return new Maximum(*this);
   }



protected:
  /* Prints this object on the given stream. */
  virtual void print(std::ostream& os) const;

private:
  /* Constructs a maximum. */
  Maximum(const Expression& operand1, const Expression& operand2)
    : Computation(operand1, operand2) {}
};


/* ====================================================================== */
/* ValueMap */

/*
 * Mapping from fluents to values.
 */
struct ValueMap : public std::map<const Fluent*, float> {
};


#endif /* EXPRESSIONS_H */

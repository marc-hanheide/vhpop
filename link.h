#ifndef LINK
#define LINK

#include "orderings.h"

struct OpenCondition;
/* ====================================================================== */
/* Link */

/*
 * Causal link.
 */
struct Link {
  /* Constructs a causal link. */
  Link(size_t from_id, StepTime effect_time, const OpenCondition& open_cond);

  /* Constructs a causal link. */
  Link(const Link& l);

  /* Deletes this causal link. */
  ~Link();

  /* Returns the id of step that link goes from. */
  size_t from_id() const { return from_id_; }

  /* Returns the time of effect satisfying link. */
  StepTime effect_time() const { return effect_time_; }

  /* Returns the id of step that link goes to. */
  size_t to_id() const { return  to_id_; }

  /* Returns the condition satisfied by link. */
  const Literal& condition() const { return *condition_; }

  /* Returns the time of the condition satisfied by this link. */
  FormulaTime condition_time() const { return condition_time_; }

private:
  /* Id of step that link goes from. */
  size_t from_id_;
  /* Time of effect satisfying link. */
  StepTime effect_time_;
  /* Id of step that link goes to. */
  size_t to_id_;
  /* Condition satisfied by link. */
  const Literal* condition_;
  /* Time of condition satisfied by link. */
  FormulaTime condition_time_;
};



/* Equality operator for links. */
inline bool operator==(const Link& l1, const Link& l2) {
  return &l1 == &l2;
}

#endif // LINK


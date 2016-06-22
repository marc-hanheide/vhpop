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

  /* Constructs a causal link. */
  Link(const Link& l, size_t task_id);

  Link(size_t from_id, StepTime effect_time, size_t to_id, const Literal * lit, FormulaTime cond_time, bool being_threaten, std::string type);

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

  void setMerged(bool b) const
  {
      being_merged=b;
  }
  bool getMerged() const
  {
      return being_merged;
  }

  void setThreaten(bool b) const
  {
      being_threaten=b;
  }
  bool getThreaten() const
  {
      return being_threaten;
  }

  void setType(std::string s)
  {
      type_ = s;
  }

  std::string getType()
   {
      return type_;
  }

  int getTaskId() const
  {
      return task_id_;
  }
  void setConditionStr(std::string str) const
  {
      condition_str_ = str;
  }

  std::string getConditionStr()
  {
      return condition_str_;
  }

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
  int task_id_;
  FormulaTime condition_time_;
  mutable bool being_merged;
  mutable bool being_threaten;
  mutable std::string type_;
  mutable std::string condition_str_;
};



/* Equality operator for links. */
inline bool operator==(const Link& l1, const Link& l2) {
  return &l1 == &l2;
}

#endif // LINK


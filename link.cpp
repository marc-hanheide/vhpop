
#include "link.h"

#include "heuristics.h"
#include "bindings.h"
#include "problems.h"
#include "domains.h"
#include "formulas.h"
#include "requirements.h"
#include "parameters.h"
#include "debug.h"
#include "flaws.h"

/* ====================================================================== */
/* Link */

/* Constructs a causal link. */
Link::Link(size_t from_id, StepTime effect_time,
       const OpenCondition& open_cond)
  : from_id_(from_id), effect_time_(effect_time), to_id_(open_cond.step_id()),
    condition_(open_cond.literal()), condition_time_(open_cond.when()) {
    being_merged = false;
    being_threaten = false;
    type_ = "";
    task_id_ = -1;
    condition_str_ = "";
  Formula::register_use(condition_);
}

Link::Link(size_t from_id, StepTime effect_time, size_t to_id, const Literal * lit, FormulaTime cond_time, bool being_threat, std::string type)
  :from_id_(from_id), effect_time_(effect_time), to_id_(to_id), condition_time_(cond_time),being_threaten(being_threat)
{
    task_id_ = -1; //TODO  check where this method is called
    type_ = type;
    condition_ = lit; //just reassigning the pointer, because lit is already copied whenever this is called
    condition_str_="";
}


/*Constructs a causal link. */
Link::Link(const Link& l)
  : from_id_(l.from_id_), effect_time_(l.effect_time_), to_id_(l.to_id_),
    condition_time_(l.condition_time_), condition_str_(l.condition_str_) {
    being_merged = l.being_merged;
    being_threaten =  l.being_threaten;
    type_ = l.type_;
    task_id_ = l.task_id_;

  if (l.condition_ !=0)
  {
     this->condition_ = l.condition_->clone();
     Formula::register_use(condition_);
  }
  else
      this->condition_ = 0;

}

Link::Link(const Link& l, size_t task_id)
    : from_id_(l.from_id_), effect_time_(l.effect_time_), to_id_(l.to_id_),
    condition_time_(l.condition_time_),condition_str_(l.condition_str_) {
    being_merged = l.being_merged;
    being_threaten =  l.being_threaten;
    type_ = l.type_;
    task_id_ = task_id;

  if (l.condition_ !=0)
  {
     this->condition_ = l.condition_->clone();
     Formula::register_use(condition_);
  }
  else
      this->condition_ = 0;


}


/* Deletes this causal link. */
Link::~Link() {
  Formula::unregister_use(condition_);
}



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
  Formula::register_use(condition_);
}

Link::Link(size_t from_id, StepTime effect_time, size_t to_id, const Literal * lit, FormulaTime cond_time, bool being_threat, std::string type)
  :from_id_(from_id), effect_time_(effect_time), to_id_(to_id), condition_time_(cond_time),being_threaten(being_threat)
{
    type_ = type;
    condition_ = lit; //just reassigning the pointer, because lit is already copied whenever this is called
}


/*Constructs a causal link. */
Link::Link(const Link& l)
  : from_id_(l.from_id_), effect_time_(l.effect_time_), to_id_(l.to_id_),
    condition_time_(l.condition_time_) {
    being_merged = l.being_merged;
    being_threaten =  l.being_threaten;
    type_ = l.type_;

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


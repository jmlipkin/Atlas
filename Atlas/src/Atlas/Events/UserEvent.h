#pragma once

#include "Event.h"

namespace Atlas {

class UserEvent : public Event {
  public:
	EVENT_CLASS_CATEGORY(EVENT_CATEGORY_USER);
};

}  // namespace Atlas
// Generated by Neo

#ifndef _COMMONS_HANDLER_H_
#define _COMMONS_HANDLER_H_

#include <commons/defines.h>

namespace commons {
class HandlerBody;

class Handler {
   public:
    Handler();
    virtual ~Handler();

    bool send(int what, int arg1, int arg2, void* obj);

    virtual bool handle(int what, int arg1, int arg2, void* obj) {
        return false;
    }

   private:
    HandlerBody* body_;
    ONLY_EMPTY_CONSTRUCTION(Handler);
};
} /* namespace: commons */

#endif /* _COMMONS_HANDLER_H_ */

# Create a new module quickly

## A simple module

```cpp
#pragma once

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFITimerModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIScheduleModule.h"

namespace ark
{

    class Example1Module : public AFIModule
    {
    public:
        explicit Example1Module() = default;

        bool Init() override;
        bool PostInit() override;
        bool Update() override;
        bool PreShut() override;
        bool Shut() override;

    protected:
        AFITimerModule* m_pTimerModule;
        AFILogModule* m_pLogModule;

        int my_test_;
    };

}
```

Explain:

- All modules inherits from `AFIModule`
- There are several conventional state machine execution functions in the module class.(Explain later)
- In addition to the variables that have their own logical functions, the main member variables of the module class are other module pointers, which are used to call the functions of other modules.(Please search **Interface oriented programming(IOP)**)

## Module execution state machine

As mentioned earlier, all of our modules are inherited from `AFIModule'. The AFIModule mainly contains the following virtual functions executed by plug-ins in different states, as follows

```cpp
namespace ark
{
    class AFIPluginManager;

    class AFIModule
    {
    public:
        AFIModule() = default;
        virtual ~AFIModule() = default;

        //Module initialization function is mainly used to 
        //find other module interface pointer used in the module.
        virtual bool Init() { return true; }

        virtual bool PostInit() { return true; }

        //Check configuration to check the legitimacy of configuration resources after module startup.
        virtual bool CheckConfig() { return true; }

        //Update pre processing functions, such as update before you want to
        //register class callbacks, DataNode callbacks, and so on.
        virtual bool PreUpdate() { return true; }

        //Loop function, which is a dead loop, is mainly used to deal with events 
        //that need to be processed all the time, such as timer, network, etc.
        virtual bool Update() { return true; }

        //Function that need to be done before closing, such as destroying resources, etc.
        virtual bool PreShut() { return true; }

        //Function that need to be done when closing, such as closing log files, etc.
        virtual bool Shut() { return true; }
    }
}
```

Module execution flowchart:

![module-flow](../_images/module-flow.png)
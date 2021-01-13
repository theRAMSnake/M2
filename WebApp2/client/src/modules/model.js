import Materia from '../modules/materia_request'

var init = false;
var workBurden = 0;
var primaryFocus = "";
var rewardModifiers = null
var onUpdateCallback = () => {}

//High level representation of materia client staff
class materiaModel
{
    static init()
    {
        if(init)
        {
            return;
        }

        init = true;

        {
            const req = {
                operation: "query",
                ids: ["work.burden"]
            };
            
            Materia.exec(req, (r) => 
            {
                workBurden = Number(r.object_list[0].value);
                onUpdateCallback();
            });
        }
        {
            const req = {
                operation: "query",
                ids: ["primary_focus"]
            };
            
            Materia.exec(req, (r) => 
            {
                primaryFocus = r.object_list[0].value;
                onUpdateCallback();
            });
        }
        {
            const req = {
                operation: "query",
                filter: "IS(reward_modifier)"
            };

            Materia.exec(req, (r) => 
            {
                rewardModifiers = r.object_list;

                onUpdateCallback();
            });
        }
    }

    static getWorkBurden()
    {
        return workBurden;
    }

    static getPrimaryFocus()
    {
        return primaryFocus;
    }

    static getRewardModifiers()
    {
        return rewardModifiers;
    }

    static setWorkBurden(newValue)
    {
        Materia.sendEdit("work.burden", JSON.stringify({value: newValue}), (cb) => { 
            if(cb.success === "1")
            {
                workBurden = Number(newValue);
                onUpdateCallback();
            }
        });
    }

    static setPrimaryFocus(newValue)
    {
        Materia.sendEdit("primary_focus", JSON.stringify({value: newValue}), (cb) => { 
            if(cb.success === "1")
            {
                primaryFocus = newValue;
                onUpdateCallback();
            }
        });
    }

    static setOnUpdateCallback(cb)
    {
        onUpdateCallback = cb;
    }
}

export default materiaModel;
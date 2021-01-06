import Materia from '../modules/materia_request'

var init = false;
var workBurden = 0;
var rewardModifier = "0%"
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
                filter: "IS(reward_modifier)"
            };

            Materia.exec(req, (r) => 
            {
                var total = 0.0;
                
                r.object_list.forEach(x => {
                    total += parseFloat(x.value)
                });

                console.log(total);
                rewardModifier = parseInt((total * 100).toString()).toString() + "%";

                onUpdateCallback();
            });
        }
    }

    static getWorkBurden()
    {
        return workBurden;
    }

    static getRewardModifier()
    {
        return rewardModifier;
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

    static setOnUpdateCallback(cb)
    {
        onUpdateCallback = cb;
    }
}

export default materiaModel;
import Materia from '../modules/materia_request'

var init = false;
var workBurden = 0;
var primaryFocus = "";
var rewardModifiers = null;
var calendarItems = null;
var tod = "";
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
        {
            const req = {
                operation: "query",
                ids: ["tip_of_the_day"]
            };
    
            Materia.exec(req, (r) => {
                var c = r;
                tod = c.object_list[0].value;

                onUpdateCallback();
            });
        }
        this.invalidateCalendar();
    }

    static getWorkBurden()
    {
        return workBurden;
    }

    static getPrimaryFocus()
    {
        return primaryFocus;
    }

    static getTod()
    {
        return tod;
    }

    static getCalendarItems()
    {
        return calendarItems;
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

    static invalidateCalendar()
    {
        {
            const req = {
                operation: "query",
                filter: "IS(calendar_item)"
            };
    
            Materia.exec(req, (r) => {
                var c = r;
                calendarItems = c.object_list;

                var ids = [];

                calendarItems.forEach(x => 
                {
                    if(x.entityTypeChoice === "StrategyNodeReference")
                    {
                        ids.push(x.nodeReference);
                    }

                    x.suffix = ""
                });

                if(ids.length > 0)
                {
                    const loadStrategyNodes = {
                        operation: "query",
                        ids: ids
                    };
            
                    Materia.exec(loadStrategyNodes, (r) => 
                    {
                        var nodes = r.object_list;

                        calendarItems.forEach(x => 
                        {
                            x.suffix = ""
                            if(x.entityTypeChoice === "StrategyNodeReference")
                            {
                                var n = nodes.find(y => {
                                    return y.id === x.nodeReference
                                });

                                if(n && n.typeChoice === "Counter")
                                {
                                    x.suffix = "(" + n.value + "/" + n.target + ")";
                                }
                            }
                        });
        
                        onUpdateCallback();
                    });
                }

                onUpdateCallback();
            });
        }
    }

    static getRandomIdea(cb)
    {
        const req = {
            operation: "random",
            typename: "idea"
        };

        Materia.exec(req, (r) => {
            cb(r.object_list[0]);
        });
    }

    static getIdeaCount(cb)
    {
        const req = {
            operation: "count",
            filter: "IS(idea)"
        };

        Materia.exec(req, (r) => {
            cb(r.result);
        });
    }


    static getIdea(id, cb)
    {
        const req = {
            operation: "query",
            ids: [id]
        };

        Materia.exec(req, (r) => {
            cb(r.object_list[0]);
        });
    }

    static getRelevantIdeas(id, cb)
    {
        const req = {
            operation: "getRelatedIdeas",
            id: id
        };

        Materia.exec(req, (r) => {
            cb(r.object_list);
        });
    }

    static searchIdeas(keyword, cb)
    {
        const req = {
            operation: "searchIdeas",
            searchString: keyword
        };

        Materia.exec(req, (r) => {
            cb(r.object_list);
        });
    }

    static createIdea(title, htgs, content)
    {
        const req = {
            operation: "create",
            typename: "idea",
            params: {
                title: title,
                hashtags: htgs,
                content: content
            }
        };

        Materia.post(req);
    }
}

export default materiaModel;
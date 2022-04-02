import Materia from '../modules/materia_request'
import MateriaConnections from '../modules/connections'

var init = false;
var workBurden = 0;
var investToSalaryRatio = 0.0;
var primaryFocus = "";
var calendarItems = null;
var currencies = null;
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
                ids: ["invest.cb"]
            };
            
            Materia.exec(req, (r) => 
            {
                investToSalaryRatio = parseFloat(r.object_list[0].percentOfSalary);
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
                filter: "IS(currency)"
            };

            Materia.exec(req, (r) => 
            {
                currencies = r.object_list;

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

    static getCurrencies()
    {
        return currencies;
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
                var calendarItemsConnections = new MateriaConnections(c.connection_list);

                var ids = [];

                calendarItems.forEach(x =>
                {
                    if(x.entityTypeChoice === "StrategyNodeReference")
                    {
                        ids = ids.concat(calendarItemsConnections.AllOf(x.id, "Refers", "*").map(x => x.B));
                    }

                    x.suffix = ""
                });

                if(ids.length > 0)
                {
                    const allRefs = {
                        operation: "query",
                        ids: ids
                    };

                    Materia.exec(allRefs, (r) =>
                    {
                        var refs = r.object_list;
                        var refConnections = new MateriaConnections(r.connection_list);

                        calendarItems.forEach(x =>
                        {
                            x.suffix = ""
                            if(x.entityTypeChoice === "StrategyNodeReference")
                            {
                                var n = refs.find(y => {
                                    return y.typename === "strategy_node" && refConnections.Has(x.id, "Refers", y.id)
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

    static getRewardItems(cb)
    {
        const req = {
            operation: "query",
            filter: "IS(reward_item)"
        };

        Materia.exec(req, (r) => {
            cb(r.object_list);
        });
    }

    static buyShopItem(id, cb)
    {
        const req = {
            operation: "buyRewardItem",
            targetId: id
        };

        Materia.exec(req, (r) => {
            cb();
        });
    }

    static getRewardShopItems(cb)
    {
        const req = {
            operation: "query",
            filter: "IS(reward_shop_item)"
        };

        Materia.exec(req, (r) => {
            cb(r.object_list);
        });
    }

    static getRewardCoins(cb)
    {
        const req = {
            operation: "query",
            ids: ["reward.coins"]
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

    static getInvestToSalaryRatio()
    {
        return investToSalaryRatio;
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

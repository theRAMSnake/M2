import Materia from '../modules/materia_request'

var init = false;
var yearlyIncome = 0;
var primaryFocus = "";
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
                ids: ["financial_report"]
            };

            Materia.exec(req, (r) =>
            {
                yearlyIncome = parseInt(r.object_list[0].balance);
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
    }


    static getPrimaryFocus()
    {
        return primaryFocus;
    }

    static getTod()
    {
        return tod;
    }

    static getCurrencies()
    {
        return currencies;
    }

    static getYearlyIncome()
    {
        return yearlyIncome;
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

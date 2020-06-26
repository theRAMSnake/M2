import MateriaRequest from '../modules/materia_request'

var types;
var init = false;

function acceptObjects(objList)
{
    if(objList.length > 0)
    {
        m3Proxy.getType(objList[0].typename).objects = objList;
    }
}

class m3Proxy
{
    static initialize()
    {
        if(init)
        {
            return;
        }

        {
            const req = {
                operation: "describe"
            };
        
            MateriaRequest.req(JSON.stringify(req), (r) => {
                types = JSON.parse(r).object_list;

                //analyze types and load referencable of them
                var i = 0;
                for(i = 0; i < types.length; ++i)
                {
                    var j = 0;
                    for (j = 0; j < types[i].fields.length; j++)
                    {
                        if(types[i].fields[j].refType.length > 0)
                        {
                            const query = {
                                operation: "query",
                                filter: "IS(" + types[i].fields[j].refType + ")"
                            };

                            MateriaRequest.req(JSON.stringify(query), (r) => {
                                acceptObjects(JSON.parse(r).object_list);
                            });
                        }
                    }
                }
            });
        }

        console.log("m3proxy init started");

        init = true;
    }

    static getTypes()
    {
        return types;
    }

    static getType(name)
    {
        var i = 0;
        for (i = 0; i < types.length; i++)
        {
            if(types[i].name === name)
            {
                return types[i];
            }
        }

        return {};
    }
}

export default m3Proxy;
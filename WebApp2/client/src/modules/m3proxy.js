import Materia from '../modules/materia_request'

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
    static initialize(successCb)
    {
        if(init)
        {
            return;
        }

        {
            const req = {
                operation: "describe"
            };

            Materia.exec(req, (r) => {
                types = r.object_list;

                //analyze types and load referencable of them
                var i = 0;
                for(i = 0; i < types.length; ++i)
                {
                    //Deprecated
                    var j = 0;
                    for (j = 0; j < types[i].fields.length; j++)
                    {
                        if(types[i].fields[j].refType.length > 0)
                        {
                            const query = {
                                operation: "query",
                                filter: "IS(" + types[i].fields[j].refType + ")"
                            };

                            Materia.exec(query, (r) => {
                                acceptObjects(r.object_list);
                            });
                        }
                    }
                    //End deprecated
                    console.log(types[i].pins);
                    if(types[i].pins)
                    {
                        j = 0;
                        for (j = 0; j < types[i].pins.length; j++)
                        {
                            const query = {
                                operation: "query",
                                filter: "IS(" + types[i].pins[j].typeNameOther + ")"
                            };

                            Materia.exec(query, (r) => {
                                acceptObjects(r.object_list);
                            });
                        }
                    }
                }

                successCb();
            });
        }

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

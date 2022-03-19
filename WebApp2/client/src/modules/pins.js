import Materia from './materia_request'
import m3proxy from './m3proxy'
import MateriaConnections from '../modules/connections.js'
export function applyPins(id, newPins, oldPins)
{
    if(!oldPins)
    {
        for(var i = 0; i < newPins.length; ++i)
        {
            if(newPins[i].id === "...")
            {
                continue;
            }
            //Assumption - all pins are references
            var link = {A: id, B: newPins[i].value, type: "Reference"}
            var linkReq = {
                operation: "create",
                typename: "connection",
                params: link
            }
            Materia.exec(linkReq, (u) => {});
        }
    }
    else
    {
        for(var i = 0; i < oldPins.length; ++i)
        {
            if(oldPins[i].value === newPins[i].value)
            {
                continue;
            }

            if(oldPins[i].value !== "...")
            {
                Materia.sendDelete(oldPins[i].connectionId, (u) => {});
            }

            if(newPins[i].value !== "...")
            {
                var link = {A: id, B: newPins[i].value, type: "Reference"}
                var linkReq = {
                    operation: "create",
                    typename: "connection",
                    params: link
                }
                Materia.exec(linkReq, (u) => {});
            }
        }
    }
}

export function getPinOptions(refType)
{
    var objects = m3proxy.getType(refType).objects.map((x) => x);
    objects.push({id: "...", name: "None"});
    return objects;
}

export function buildPinsTemplate(typename)
{
    var result = [];

    const type = m3proxy.getType(typename);
    if(type.pins)
    {
        var j = 0;
        for (j = 0; j < type.pins.length; j++)
        {
            result.push(type.pins[j]);
            result[result.length - 1].value = "...";
        }
    }

    return result;
}

export function makePins(obj, cb)
{
    var pins = buildPinsTemplate(obj.typename);

    if(pins.length == 0)
    {
        cb(pins);
    }
    else
    {
        const req = {
            operation: "query",
            ids: [obj.id]
        };

        Materia.exec(req, (r) => {
            if(r.connection_list === "")
            {
                cb(pins);
            }
            else
            {
                var conns = new MateriaConnections(r.connection_list);

                if(pins.length == 1)
                {
                    //For simplicty this code is implemented for pins of size 1 only
                    var refs = conns.All("Reference")
                    for(var i = 0; i < refs.length; ++i)
                    {
                        const ref_req = {
                            operation: "query",
                            ids: [refs[i].B]
                        };

                        function changeRef(id, pins, cb)
                        {
                            return (r) => {
                                if(pins[0].typeNameOther === r.object_list[0].typename)
                                {
                                   pins[0].value = r.object_list[0].id;
                                   pins[0].connectionId = id;
                                   cb(pins);
                                }
                            }
                        }

                        Materia.exec(ref_req, changeRef(refs[i].id, pins, cb));
                    }
                }
                else
                {
                    console.log("Assumption violation");
                }
            }
        });
    }
}


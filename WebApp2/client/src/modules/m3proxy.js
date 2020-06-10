import MateriaRequest from '../modules/materia_request'

var types;
var init = false;

class m3Proxy
{
    static initialize()
    {
        if(init)
        {
            return;
        }

        const req = {
            operation: "describe"
        };
    
        MateriaRequest.req(JSON.stringify(req), (r) => {
            types = JSON.parse(r).object_list;
        });

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
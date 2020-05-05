import MateriaRequest from '../modules/materia_request'

var traits;
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
            operation: "query",
            filter: "IS(trait)"
        };
    
        MateriaRequest.req(JSON.stringify(req), (r) => {
            traits = JSON.parse(r).object_list;
        });

        console.log("m3proxy init started");

        init = true;
    }

    static getTraits()
    {
        return traits;
    }
}

export default m3Proxy;
import Materia from '../modules/materia_request'

export default function queryChain(chainArray, successCb)
{
    var cb = chainArray[0];

    Materia.exec(cb.req, (rsp) => 
    {
        cb.step(rsp);

        chainArray.splice(0, 1);
        if(chainArray.length != 0)
        {
            queryChain(chainArray, successCb);
        }   
        else
        {
            successCb();
        }     
    });
}
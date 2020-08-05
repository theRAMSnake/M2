import Auth from '../modules/auth';

var globalErrorHandler;

class Materia
{
    static req(op, cb, errCb)
    {
        const xhr = new XMLHttpRequest();
        xhr.open('post', '/api/materia');
        xhr.setRequestHeader('Content-type', 'application/json;charset=UTF-8');
        xhr.setRequestHeader('Authorization', `bearer ${Auth.getToken()}`);
        xhr.responseType = 'json';
        xhr.addEventListener('load', () => {
        if (xhr.status === 200) {
            cb(xhr.response.message);
        }
        else if(xhr.status === 401)
        {
            Auth.deauthenticateUser();
        }
        else 
        {
            errCb(xhr.statusText);
        }
        });
        xhr.send(op);
    }

    static exec(cmd, cb)
    {
        Materia.req(JSON.stringify(cmd), (resp) => {cb(JSON.parse(resp))}, (errText) => {globalErrorHandler && globalErrorHandler(errText)});
    }

    static setGlobalErrorHandler(hndl)
    {
        globalErrorHandler = hndl;
    }

    static postDelete(s)
    {
        var rq = {
            operation: "destroy",
            id: s
        };

        Materia.exec(rq, () => {});
    }

    static postComplete(s)
    {
        var rq = {
            operation: "complete",
            id: s
        };

        Materia.exec(rq, () => {});
    }

    static post(rq)
    {
        Materia.exec(rq, () => {});
    }

    static postEdit(id, s)
    {
        var rq = {
            operation: "modify",
            id: id,
            params: JSON.parse(s)
        };

        Materia.exec(rq, () => {});
    }
}

export default Materia;
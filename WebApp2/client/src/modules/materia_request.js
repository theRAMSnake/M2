import Auth from '../modules/Auth';

class MateriaRequest
{
    static req(op, cb)
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
        else 
        {
            cb(xhr.statusText);
        }
        });
        xhr.send(op);
    }
}

export default MateriaRequest;
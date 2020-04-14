import Auth from '../modules/Auth';

class MateriaRequest
{
    static req(op)
    {
        const xhr = new XMLHttpRequest();
        xhr.open('post', '/api/materia');
        xhr.setRequestHeader('Content-type', 'application/json;charset=UTF-8');
        xhr.setRequestHeader('Authorization', `bearer ${Auth.getToken()}`);
        xhr.responseType = 'json';
        xhr.addEventListener('load', () => {
        if (xhr.status === 200) {
            alert(xhr.response.message);
        }
        else {
            alert("not authorized");
        }
        });
        xhr.send(JSON.stringify(op));
    }
}

export default MateriaRequest;
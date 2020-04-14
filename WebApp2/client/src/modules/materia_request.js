import Auth from '../modules/Auth';

class MateriaRequest
{
    static req()
    {
        const xhr = new XMLHttpRequest();
        xhr.open('get', '/api/test');
        xhr.setRequestHeader('Content-type', 'application/json');
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
        xhr.send();
    }
}

export default MateriaRequest;
var callback;

class Auth {

    /**
     * Authenticate a user. Save a token string in Local Storage
     *
     * @param {string} token
     */
    static authenticateUser(token) {
      localStorage.setItem('token', token);
      callback(true);
    }
  
    /**
     * Check if a user is authenticated - check if a token is saved in Local Storage
     *
     * @returns {boolean}
     */
    static isUserAuthenticated() {
      return localStorage.getItem('token') !== null;
    }
  
    /**
     * Deauthenticate a user. Remove a token from Local Storage.
     *
     */
    static deauthenticateUser() {
      localStorage.removeItem('token');
      callback(false);
    }
  
    /**
     * Get a token value.
     *
     * @returns {string}
     */
  
    static getToken() {
      return localStorage.getItem('token');
    }

    static setCallback(cb){
      callback = cb;
    }
  }
  
  export default Auth;
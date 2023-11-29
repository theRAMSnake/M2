import Materia from '../modules/materia_request'

class ScriptHelper
{

static loadCollection (colName, cb, cbError) {
   let script = "import views\nresult = views.collection_to_json('" + colName + "')"
   Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
      let result = JSON.parse(r);
      if(result.result) {
         cb(JSON.parse(result.result));
      } else {
         cbError(result.error);
      }
   }, (err) => {
      cbError(err);
   });
};

static exec (script, cb) {
   Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
      let result = JSON.parse(r);
      if(result.result) {
         cb(JSON.parse(result.result));
      } else {
          console.log(result.error);
      }
   });
};

static exec_string (script, cb) {
   Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
      let result = JSON.parse(r);
      if(result.result !== null) {
         cb(result.result);
      } else {
          console.log(result.error);
      }
   });
};
};

export default ScriptHelper;

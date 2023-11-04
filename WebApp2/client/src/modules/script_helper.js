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

};

export default ScriptHelper;

import React, { useState, useEffect } from 'react';
import Materia from '../modules/materia_request'

const removePrivateKeys = (data) => {
    return data.map((item) => {
      // Create a new object excluding keys that start with '_'.
      const filteredItem = Object.keys(item)
        .filter(key => !key.startsWith('_'))
        .reduce((obj, key) => {
          obj[key] = item[key];
          return obj;
        }, {});
      return filteredItem;
    });
  };

const loadContent = (colName, cb, cbError) => {
   let script = "import views\nresult = views.collection_to_json('" + colName + "')"
   Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
      let result = JSON.parse(r);
      if(result.result) {
         cb(removePrivateKeys(JSON.parse(result.result)));
      } else {
         cbError(result.error);
      }
   }, (err) => {
      cbError(err);
   });
};

const CollectionView = ({ colName }) => {
  const [content, setContent] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState('');

  useEffect(() => {
    // If colName starts with "=", we remove that character.
    const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;

    // Load content using the provided function and colName.
    loadContent(adjustedColName, (data) => {
      setContent(data);  // set the content
      setIsLoading(false); // indicate that loading has completed
    }, (error) => {
      setError(error); // set the error message
      setIsLoading(false); // indicate that loading has completed, even though it's with an error
    });
  }, [colName]); // re-run the effect if colName changes

  // Render a loading indicator if the content is still being loaded.
  if (isLoading) {
    return <div>Loading...</div>;
  }

  // If there is an error, display it instead of the content.
  if (error) {
    return <div>Error: {error}</div>;
  }

  // Render the content as a table.
  return (
    <div>
      <h1>Collection: {colName}</h1>
      <table>
        <thead>
          <tr>
            {/* Render table headers based on the object keys */}
            {content[0] && Object.keys(content[0]).map((key) => <th key={key}>{key}</th>)}
          </tr>
        </thead>
        <tbody>
          {/* Render each item as a row in the table */}
          {content.map((item, index) => (
            <tr key={index}>
              {Object.values(item).map((value, i) => <td key={i}>{value}</td>)} {/* Replace 'i' with a unique key if available */}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default CollectionView;

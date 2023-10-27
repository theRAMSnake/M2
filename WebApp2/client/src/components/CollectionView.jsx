import React, { useState, useEffect } from 'react';
import { styled } from '@mui/material/styles';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell, { tableCellClasses } from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TableRow from '@mui/material/TableRow';
import Typography from '@mui/material/Typography';
import Paper from '@mui/material/Paper';
import Materia from '../modules/materia_request'
import Dialog from '@material-ui/core/Dialog';
import Button from '@material-ui/core/Button';
import AceEditor from 'react-ace';
import 'ace-builds/src-noconflict/mode-json';
import 'ace-builds/src-noconflict/theme-monokai';

const StyledTableCell = styled(TableCell)(({ theme }) => ({
  [`&.${tableCellClasses.head}`]: {
    backgroundColor: theme.palette.common.black,
    color: theme.palette.common.white,
  },
  [`&.${tableCellClasses.body}`]: {
    fontSize: 14,
  },
}));

const Header = styled(Typography)({
  marginBottom: '20px',
});

const StyledTableRow = styled(TableRow)(({ theme }) => ({
  '&:nth-of-type(odd)': {
    backgroundColor: theme.palette.action.hover,
  },
  // hide last border
  '&:last-child td, &:last-child th': {
    border: 0,
  },
}));

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
  const [isOpen, setIsOpen] = useState(false);
  const [editedJson, setEditedJson] = useState({});
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState('');

  const removeUnwantedFields = (data) => {
    // this doesn't remove the fields from 'content', it just prevents them from being displayed
    return data.map((item) => {
      const { id, modified, typename, ...rest } = item;
      return rest;
    });
  };

  const handleOpenDialog = (index) => {
    setIsOpen(true);
    setEditedJson(content[index]);
  };

  const handleCloseDialog = () => {
    setIsOpen(false);
  };

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

  const columns = content.length > 0 ? Object.keys(content[0]).filter(key => key !== 'id' && key !== 'modified' && key !== 'typename') : [];

  // Render the content as a table.
  return (
    <div style={{ margin: '0 3%' }}>
      <Dialog open={isOpen} onClose={handleCloseDialog}>
        <div>
          <h2>Edit JSON</h2>
          <AceEditor
            mode="json"
            theme="monokai" // Choose your preferred theme
            //onChange={(newValue) => setEditedJson(newValue)}
            name="json-editor"
            editorProps={{ $blockScrolling: true }}
            value={editedJson}
          />
          <Button variant="contained" color="primary" onClick={handleSave}>
            Save
          </Button>
          <Button variant="contained" color="secondary" onClick={handleCloseDialog}>
            Cancel
          </Button>
        </div>
      </Dialog>
      <Header variant="h4" align="center" color="primary">
        {colName.replace(/^=/, '')}
      </Header>
      <TableContainer component={Paper}>
          <Table sx={{ minWidth: 700 }} aria-label="customized table">
            <TableHead>
              <TableRow>
                  {columns.map((column) => (
                    <StyledTableCell>{column}</StyledTableCell>
                  ))}
              </TableRow>
            </TableHead>
            <TableBody>
              {removeUnwantedFields(content).map((row, index) => (
                <StyledTableRow key={index} hover sx={{ cursor: 'pointer' }} onClick={() => handleOpenDialog(index)}>
                    {columns.map((column) => (
                      <StyledTableCell>{row[column]}</StyledTableCell>
                    ))}
                </StyledTableRow>
              ))}
            </TableBody>
          </Table>
        </TableContainer>
    </div>
  );
};

export default CollectionView;

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
import Grid from '@mui/material/Grid';
import IconButton from '@mui/material/IconButton';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import Materia from '../modules/materia_request'
import Dialog from '@material-ui/core/Dialog';
import Button from '@material-ui/core/Button';
import AceEditor from 'react-ace';
import SaveIcon from '@material-ui/icons/Save';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import Fab from '@mui/material/Fab';
import Checkbox from '@mui/material/Checkbox';
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
  const [selected, setSelected] = useState([]);
  const [isOpen, setIsOpen] = useState(false);
  const [changed, setChanged] = useState(false);
  const [isAdd, setIsAdd] = useState(false);
  const [editedJson, setEditedJson] = useState("");
  const [index, setIndex] = useState(0);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState('');

  const removeUnwantedFields = (data) => {
    // this doesn't remove the fields from 'content', it just prevents them from being displayed
    return data.map((item) => {
      const { id, modified, typename, ...rest } = item;
      return rest;
    });
  };

  const handleModify = (index) => {
    setIsOpen(true);
    setIsAdd(false);
    setIndex(index);
    setEditedJson(JSON.stringify(content[index], null, 2));
  };

  const handleSelecteAll = (index) => {
      let newSelected = selected.slice().fill(!selected.some(element => element === true));
      setSelected(newSelected);
  };

  const handleSelected = () => {
      let newSelected = selected.slice();
      newSelected[index] = !newSelected[index];
      setSelected(newSelected);
  };

  const handleCloseDialog = () => {
    setIsOpen(false);
  };

  const stripElement = (obj) => {
      let newobj = {}
      for (const key in obj) {
          if (key !== 'id' && key !== 'modified' && key !== 'typename') {
             newobj[key] = ""; // Set the value to an empty string
          }
      }
      return newobj
  }

  const makeEmptyObject = (element) => {
      {}
  }

  const handleAdd = () => {
    setIsAdd(true);
    setIsOpen(true);
    setIndex(-1);
    if(content.length > 0) {
       setEditedJson(JSON.stringify(stripElement(content[0]), null, 2));
    } else {
       setEditedJson(JSON.stringify(makeEmptyObject()));
    }
  }
  const handleDelete = () => {
  }

  const jsonToM4O = (name, json) => {
      let res = name + " = m4.MateriaObject()\n";
      let ob = JSON.parse(json);
      for (const key in ob) {
          res = res + name + "." + key + "='" + ob[key] + "'\n"
      }
      return res;
  }

  const handleSave = () => {
    const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;
    setIsOpen(false);
    setChanged(false);
    if(isAdd) {
       let script = "import collection\nimport m4\n" + jsonToM4O("a", editedJson) + "\ncol = collection.Collection('" + adjustedColName + "')\ncol.add(a)\nresult = 1";
       Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
          let result = JSON.parse(r);
          if(result.result) {
             const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;
             loadContent(adjustedColName, (data) => {
               let falsesArray = Array.from(data, () => false);
               setSelected(falsesArray);
               setContent(data);  // set the content
               setIsLoading(false); // indicate that loading has completed
             }, (error) => {
               setError(error); // set the error message
               setIsLoading(false); // indicate that loading has completed, even though it's with an error
             });
          } else {
             setError(result.error); // set the error message
          }
       });
    } else {
        Materia.postEdit(content[index].id, editedJson);
        content[index] = JSON.parse(editedJson);
        setContent(JSON.parse(JSON.stringify(content)));
    }
  }

  useEffect(() => {
    // If colName starts with "=", we remove that character.
    const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;

    // Load content using the provided function and colName.
    loadContent(adjustedColName, (data) => {
      let falsesArray = Array.from(data, () => false);
      setSelected(falsesArray);
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
      <Dialog open={isOpen}
        onClose={handleCloseDialog}
        maxWidth="md"
        fullWidth
        PaperProps={{
          style: {
            width: '50%',
            height: '70%',
            display: 'flex',
            flexDirection: 'column',
            justifyContent: 'center',
            alignItems: 'center',
          },
        }}
      >
        <Paper elevation={3} style={{ height: '100%', width: '100%' }}>
          <AceEditor
            mode="json"
            theme="monokai" // Choose your preferred theme
            onChange={(newValue) => {setEditedJson(newValue);setChanged(true);}}
            name="json-editor"
            editorProps={{ $blockScrolling: true }}
            value={editedJson}
            height="100%"
            showPrintMargin={false}
            width="100%"
            setOptions={{
              tabSize: 2, // Adjust the tab size for indentation
              useSoftTabs: true, // Use soft tabs (spaces) for indentation
              wrap: true, // Enable line wrapping
            }}
          />
          {changed && <Fab sx={{position: 'absolute', bottom: 16, right: 16}} color="primary" onClick={() => handleSave()}>
            <SaveIcon/>
          </Fab>}
        </Paper>
      </Dialog>
      <Header variant="h4" align="center" color="primary">
        {colName.replace(/^=/, '')}
      </Header>
      <Grid container direction="row" justify="space-around" alignItems="flex-start">
          <IconButton edge="start" aria-label="complete" onClick={() => handleAdd()} color="primary">
            <AddCircleOutlineIcon/>
          </IconButton>
          <IconButton edge="start" aria-label="complete" onClick={() => handleDelete()} disabled={selected.some(element => element === false)} color="primary">
            <DeleteForeverIcon/>
          </IconButton>
          <TableContainer component={Paper}>
              <Table sx={{ minWidth: 700 }} aria-label="customized table">
                <TableHead>
                  <TableRow>
                      <StyledTableCell padding="checkbox" onClick={() => handleSelecteAll(index)}>
                          <Checkbox
                            color="primary"
                          />
                      </StyledTableCell>
                      {columns.map((column) => (
                        <StyledTableCell>{column}</StyledTableCell>
                      ))}
                  </TableRow>
                </TableHead>
                <TableBody>
                  {removeUnwantedFields(content).map((row, index) => (
                    <StyledTableRow key={index} hover sx={{ cursor: 'pointer' }} >
                        <StyledTableCell padding="checkbox" onClick={() => handleSelected(index)}>
                          <Checkbox
                            color="primary"
                            checked={selected[index]}
                          />
                        </StyledTableCell>
                        {columns.map((column) => (
                          <StyledTableCell onClick={() => handleModify(index)}>{row[column]}</StyledTableCell>
                        ))}
                    </StyledTableRow>
                  ))}
                </TableBody>
              </Table>
            </TableContainer>
      </Grid>
    </div>
  );
};

export default CollectionView;

import React, { useState, useEffect } from 'react';
import { styled } from '@mui/material/styles';
import TableCell, { tableCellClasses } from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import Materia from '../modules/materia_request'
import ScriptHelper from '../modules/script_helper'
import Dialog from '@material-ui/core/Dialog';
import Button from '@material-ui/core/Button';
import AceEditor from 'react-ace';
import SaveIcon from '@material-ui/icons/Save';
import EditIcon from '@material-ui/icons/Edit';
import AddCircleOutlineIcon  from '@material-ui/icons/AddCircleOutline';
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import IngestionDialog from './dialogs/IngestionDialog.jsx'
import EditorDialog from './dialogs/EditorDialog.jsx'
import 'ace-builds/src-noconflict/mode-json';
import 'ace-builds/src-noconflict/theme-monokai';
import {
    Checkbox,
    Fab,
    IconButton,
    Paper,
    Grid,
    Typography,
    TableRow,
    TableHead,
    Table,
    TableBody
} from "@material-ui/core";

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
  const [objectInEdit, setObjectInEdit] = useState('');
  const [inDeleteDialog, setInDeleteDialog] = useState(false);
  const [inHeaderDialog, setInHeaderDialog] = useState(false);

  const removeUnwantedFields = (data) => {
    // this doesn't remove the fields from 'content', it just prevents them from being displayed
    return data.map((item) => {
      const { id, modified, typename, ...rest } = item;
      return rest;
    });
  };

  function onDeleteDialogCancel()
  {
      setInDeleteDialog(false);
  }

  function onDeleteDialogOk()
  {
      setInDeleteDialog(false);
      let newContent = JSON.parse(JSON.stringify(content));
      newContent.items = [];
      for (let i = 0; i < selected.length; i++) {
          if(selected[i]) {
              Materia.postDelete(content.items[i].id);
          } else {
              newContent.items.push(content.items[i]);
          }
      }
      setContent(newContent);
      let falsesArray = Array.from(newContent.items, () => false);
      setSelected(falsesArray);
  }

  const handleModify = (index) => {
    setIsOpen(true);
    setIsAdd(false);
    setIndex(index);
    setEditedJson(JSON.stringify(content.items[index], null, 2));
    setObjectInEdit(content.items[index]);
  };

  const handleSelecteAll = () => {
      let newSelected = selected.slice().fill(!selected.some(element => element === true));
      setSelected(newSelected);
  };

  const handleSelected = (index) => {
      let newSelected = selected.slice();
      newSelected[index] = !newSelected[index];
      setSelected(newSelected);
  };

  const handleCloseDialog = () => {
    setIsOpen(false);
  };

  const handleCloseHeaderDialog = () => {
    setInHeaderDialog(false);
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
    setObjectInEdit(null);
    setIndex(-1);
    if(content.items.length > 0) {
       setEditedJson(JSON.stringify(stripElement(content.items[0]), null, 2));
    } else {
       setEditedJson(JSON.stringify(makeEmptyObject()));
    }
  }
  const handleDelete = () => {
      setInDeleteDialog(true);
  }

  const handleEditHeader = () => {
      setInHeaderDialog(true);
  }

  const handleHeaderDialogSave = (text) => {
      setInHeaderDialog(false);
      Materia.postEdit(content.header.id, text);
      content.header = JSON.parse(text);
      setContent(JSON.parse(JSON.stringify(content)));
  }

  const handleSave = (text) => {
    const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;
    setIsOpen(false);
    setChanged(false);
    if(isAdd) {
       let script = "import collection\nimport m4\n" + ScriptHelper.jsonToM4O("a", text) + "\ncol = collection.Collection('" + adjustedColName + "')\ncol.add(a)\nresult = 1";
       Materia.req(JSON.stringify({ operation: "run", script: script }), (r) => {
          let result = JSON.parse(r);
          if(result.result) {
             const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;
             ScriptHelper.loadCollection(adjustedColName, (data) => {
               let falsesArray = Array.from(data.items, () => false);
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
        Materia.postEdit(content.items[index].id, text);
        content.items[index] = JSON.parse(text);
        setContent(JSON.parse(JSON.stringify(content)));
    }
  }

  useEffect(() => {
    // If colName starts with "=", we remove that character.
    const adjustedColName = colName.startsWith('=') ? colName.slice(1) : colName;

    // Load content using the provided function and colName.
    ScriptHelper.loadCollection(adjustedColName, (data) => {
      let falsesArray = Array.from(data.items, () => false);
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

  const columns = content.items.length > 0 ? Object.keys(content.items[0]).filter(key => key !== 'id' && key !== 'modified' && key !== 'typename') : [];

  // Render the content as a table.
  return (
    <div style={{ margin: '0 3%' }}>
      <ConfirmationDialog open={inDeleteDialog} question="delete" caption="confirm delete" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
      {isOpen && !content.header.ingestion && <EditorDialog onClose={handleCloseDialog} text={editedJson} onSave={handleSave} />}
      {isOpen && content.header.ingestion && <IngestionDialog open={true} ingestion={content.header.ingestion} object={objectInEdit} onCancel={handleCloseDialog} onOk={handleSave} />}
      {inHeaderDialog && <EditorDialog onClose={handleCloseHeaderDialog} text={JSON.stringify(content.header, null, 2)} onSave={handleHeaderDialogSave} />}
      <Header variant="h4" align="center" color="primary">
        {colName.replace(/^=/, '') + "(" + content.items.length + ")"}
      </Header>
      <Grid container direction="row" alignItems="flex-start">
          <IconButton edge="start" aria-label="complete" onClick={() => handleAdd()}>
            <AddCircleOutlineIcon/>
          </IconButton>
          <IconButton edge="start" aria-label="complete" onClick={() => handleDelete()} disabled={!selected.some(element => element === true)} >
            <DeleteForeverIcon/>
          </IconButton>
          <IconButton edge="start" aria-label="complete" onClick={() => handleEditHeader()} >
            <EditIcon/>
          </IconButton>
          <TableContainer component={Paper}>
              <Table sx={{ minWidth: 700 }} aria-label="customized table">
                <TableHead>
                  <TableRow>
                      <StyledTableCell padding="checkbox" onClick={() => handleSelecteAll()}>
                          <Checkbox
                            color="primary"
                            checked={selected.some(element => element === true)}
                          />
                      </StyledTableCell>
                      {columns.map((column) => (
                        <StyledTableCell>{column}</StyledTableCell>
                      ))}
                  </TableRow>
                </TableHead>
                <TableBody>
                  {removeUnwantedFields(content.items).map((row, index) => (
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

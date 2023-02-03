import React, { useState } from 'react';
import Materia from '../modules/materia_request'
import ReactFlow from 'reactflow';
import { MarkerType, Position } from 'reactflow';
import 'reactflow/dist/style.css';
import '../../css/strategy.css';
import {
  addEdge,
  MiniMap,
  Controls,
  Background,
  useNodesState,
  useEdgesState,
} from 'reactflow';
import PathCtrl from './PathCtrl.jsx'
import m3proxy from '../modules/m3proxy'
import QueryChain from '../modules/QueryChain'

import MateriaConnections from '../modules/connections'
import AddCircleOutlineIcon from '@material-ui/icons/AddCircleOutline';
import DeleteIcon from '@material-ui/icons/Delete';
import ClearAllIcon from '@material-ui/icons/ClearAll';
import SwapHorizIcon from '@material-ui/icons/SwapHoriz';
import SwapVertIcon from '@material-ui/icons/SwapVert';
import LinkIcon from '@material-ui/icons/Link';
import SaveIcon from '@material-ui/icons/Save';
import DoneIcon from '@material-ui/icons/Done';
import CalendarTodayIcon from '@material-ui/icons/CalendarToday';
import ConfirmationDialog from './dialogs/ConfirmationDialog.jsx'
import WatchLaterIcon  from '@material-ui/icons/WatchLater';
import DateTimeCtrl from './DateTimeCtrl.jsx'
import AddItemDialog from './AddItemDialog.jsx'

import {
    CircularProgress,
    Backdrop,
    Grid,
    Typography,
    IconButton,
    Dialog,
    DialogTitle,
    DialogContent,
    TextField,
    FormControl,
    InputLabel,
    Select,
    Button,
    DialogActions,
    Divider,
    FormControlLabel,
    Checkbox
} from "@material-ui/core";

function NodeInfoView(props)
{
    const [changed, setChanged] = useState(false);
    const [inCompleteDialog, setInCompleteDialog] = useState(false);
    const [title, setTitle] = useState(props.node.title);
    const [node, setNode] = useState(props.node);
    const [details, setDetails] = useState(props.node.details);
    const [reward, setReward] = useState(props.node.reward);
    const isAchieved = props.node.isAchieved === 'true';
    const [value, setValue] = useState(props.node.value);
    const [target, setTarget] = useState(props.node.target);
    const [coreValue, setCoreValue] = useState(props.coreValue);
    const [date, setDate] = useState(props.node.date);

    if(props.node != node)
    {
        setDetails(props.node.details);
        setReward(props.node.reward);
        setTitle(props.node.title);
        setValue(props.node.value);
        setTarget(props.node.target);
        setDate(props.node.date);
        setNode(props.node);
        setCoreValue(props.coreValue);
    }

    function onSaveClicked()
    {
        setChanged(false);
        props.node.title = title;
        props.node.details = details;
        props.node.reward = reward;
        props.node.value = value;
        props.node.target = target;
        props.node.date = date;
        props.coreValue = coreValue;
        props.onChanged(props.node, props.coreValue);
    }

    function handleTitleChange(e)
    {
        setTitle(e.target.value);
        setChanged(true);
    }

    function handleNotesChange(e)
    {
        setDetails(e.target.value);
        setChanged(true);
    }

    function handleRewardChange(e)
    {
        setReward(parseInt(e.target.value));
        setChanged(true);
    }

    function handleValueChange(e)
    {
        setValue(parseInt(e.target.value));
        setChanged(true);
    }

    function handleTargetChange(e)
    {
        setTarget(parseInt(e.target.value));
        setChanged(true);
    }

    function handleCoreValueChange(e)
    {
        setCoreValue(e.target.value);
        setChanged(true);
    }

    function onCompleteDialogCancel()
    {
        setInCompleteDialog(false);
    }

    function onCompleteDialogOk()
    {
        setInCompleteDialog(false);
        props.onCompleted();
    }

    function completeClicked()
    {
        setInCompleteDialog(true);
    }

    function handleDTChange(val, id)
    {
        setDate(val);
        setChanged(true);
    }

    return (
        <div>
            <ConfirmationDialog open={inCompleteDialog} question="complete" caption="confirm completion" onNo={onCompleteDialogCancel} onYes={onCompleteDialogOk} />
            <IconButton edge="start" aria-label="complete" onClick={props.addClicked}>
                <AddCircleOutlineIcon/>
            </IconButton>
            <IconButton edge="start" aria-label="complete" onClick={props.addLinkClicked}>
                <LinkIcon/>
            </IconButton>
            <IconButton edge="start" aria-label="complete" onClick={props.addCalendarClicked}>
                <CalendarTodayIcon/>
            </IconButton>
            <IconButton edge="start" aria-label="delete" onClick={props.deleteClicked}>
                <DeleteIcon />
            </IconButton>
            <IconButton edge="start" onClick={onSaveClicked} disabled={!changed}>
                <SaveIcon/>
            </IconButton>
            <IconButton edge="start" onClick={props.onSplitVertClicked}>
                <SwapVertIcon/>
            </IconButton>
            <IconButton edge="start" onClick={props.onSplitHorizClicked}>
                <SwapHorizIcon/>
            </IconButton>
            {(node.typeChoice === "Goal" || node.typeChoice === "Task" || (node.typeChoice == "Counter" && node.value >= node.target)) && !isAchieved && <IconButton edge="end" aria-label="complete" onClick={completeClicked}>
                <DoneIcon/>
            </IconButton>}
            <Divider/>
            <Grid container direction="column" justify="flex-start" alignItems="center">
                <Typography color={isAchieved ? "primary" : "white"} variant="h6">{isAchieved ? "Achieved" : "In Progress"}</Typography>
            </Grid>
            <TextField inputProps={{onChange: handleTitleChange}} value={title} fullWidth id="title" label="Title" />
            <FormControl fullWidth style={{marginTop: '10px'}}>
                <InputLabel htmlFor="CoreValue">Core Value</InputLabel>
                    <Select
                        native
                        value={coreValue}
                        onChange={handleCoreValueChange}
                        inputProps={{
                            name: "Type",
                            id: "Type",
                        }}
                        >
                        {GetCoreValueTypes().map((obj, index) => <option aria-label="None" value={obj} key={index} >{obj}</option>)}
                    </Select>
            </FormControl>
            {!isAchieved && node.typeChoice !== "Watch" && node.type != "Wait" && <TextField inputProps={{onChange: handleRewardChange, type: 'number'}} value={reward} fullWidth id="reward" label="Reward" />}
            {!isAchieved && node.typeChoice === "Counter" && <Grid container direction="row" justify="space-around" alignItems="center">
                    <TextField inputProps={{onChange: handleValueChange, type: 'number', style: { width: "150px" }}} value={value} id="value" label="Value" />
                    <TextField inputProps={{onChange: handleTargetChange, type: 'number', style: { width: "150px" }}} value={target} id="target" label="Target" />
                </Grid>}
            {!isAchieved && node.typeChoice === "Wait" && <DateTimeCtrl onChange={handleDTChange} value={date} id="date"/>}
            <TextField inputProps={{onChange: handleNotesChange}} multiline={true} rows="25" value={details} fullWidth id="notes" label="Details" />
        </div>
        );
}

function GetNodeTypes()
{
    return m3proxy.getType("strategy_node").fields.find(x => x.name === "typeChoice").options;
}

function GetCoreValueTypesWithParent()
{
    var types = GetCoreValueTypes();
    types.unshift("Parent");
    return types;
}

function GetCoreValueTypes()
{
    var objects = m3proxy.getType("core_value").objects.map((x) => x.name);
    objects.unshift("None");
    return objects;
}

function AddNodeDialog(props)
{
    const [title, setTitle] = useState("");
    const [type, setType] = useState("Task");
    const [coreValue, setCoreValue] = useState("Parent");
    const [target, setTarget] = useState(0);
    const [date, setDate] = useState(Math.floor(new Date() / 1000));

    function onTitleChanged(e)
    {
        setTitle(e.target.value);
    }

    function typeChange(e)
    {
        setType(e.target.value);
    }

    function coreValueChange(e)
    {
        setCoreValue(e.target.value);
    }

    function handleTargetChange(e)
    {
        setTarget(parseInt(e.target.value));
    }

    function handleDTChange(val, id)
    {
        setDate(val);
    }

    return (
        <Dialog open={true} onClose={props.onClose} aria-labelledby="dialog-title">
            <DialogTitle id="dialog-title">Create new node</DialogTitle>
            <DialogContent>
                <TextField id="id_title" label="Title" fullWidth  inputProps={{onChange: onTitleChanged}}/>
                <FormControl fullWidth style={{marginTop: '10px'}}>
                    <InputLabel htmlFor="Type">Type</InputLabel>
                        <Select
                            native
                            value={type}
                            onChange={typeChange}
                            inputProps={{
                                name: "Type",
                                id: "Type",
                            }}
                            >
                            {GetNodeTypes().map((obj, index) => <option aria-label="None" value={obj} key={index} >{obj}</option>)}
                        </Select>
                </FormControl>
                {type === "Counter" && <TextField inputProps={{onChange: handleTargetChange, type: 'number'}} value={target} fullWidth id="target" label="Target" />}
                {type === "Wait" && <DateTimeCtrl onChange={handleDTChange} value={date} id="date"/>}
                <FormControl fullWidth style={{marginTop: '10px'}}>
                    <InputLabel htmlFor="CoreValue">Core Value</InputLabel>
                        <Select
                            native
                            value={coreValue}
                            onChange={coreValueChange}
                            inputProps={{
                                name: "Type",
                                id: "Type",
                            }}
                            >
                            {GetCoreValueTypesWithParent().map((obj, index) => <option aria-label="None" value={obj} key={index} >{obj}</option>)}
                        </Select>
                </FormControl>
            </DialogContent>
            <DialogActions>
                <Button onClick={props.onClose} variant="contained" color="primary">
                    Cancel
                </Button>
                <Button onClick={() => props.onOk(title, type, target, date, coreValue)} variant="contained" color="primary" autoFocus>
                    Ok
                </Button>
            </DialogActions>
        </Dialog>
    );
}

function WatchPanel(props)
{
    const [inCompleteDialog, setInCompleteDialog] = useState(false);
    const [nodeId, setNodeId] = useState("");

    function onCompleteDialogCancel()
    {
        setInCompleteDialog(false);
    }

    function onCompleteDialogOk()
    {
        setInCompleteDialog(false);
        props.completed(nodeId);
    }

    function completeClicked(id)
    {
        setInCompleteDialog(true);
        setNodeId(id);
    }

    return (
        <Grid container direction="row" justify="flex-start" alignItems="center">
        <ConfirmationDialog open={inCompleteDialog} question="complete" caption="confirm completion" onNo={onCompleteDialogCancel} onYes={onCompleteDialogOk} />
        {props.nodes.map(x => {
            return <div style={{paddingRight: "10px"}}>
                <WatchLaterIcon size='small'/>
                <div style={{position: 'relative', top: "-7px", left: "5px", display:'inline'}}>
                    {x.orig.title}
                </div>
                <IconButton edge="end" size='small' aria-label="complete" style={{position: 'relative', top: "-7px", left: "5px", display:'inline'}}>
                    <DoneIcon fontSize='small' onClick={() => {completeClicked(x.id)}}/>
                </IconButton>
            </div>
        })}
    </Grid>);
}

function StrategyView(props)
{
    const [updating, setUpdating] = useState(true);
    const [nodes, setNodes, onNodesChange] = useNodesState(null);
    const [edges, setEdges, onEdgesChange] = useEdgesState(null);
    const [path, setPath] = useState([]);
    const [initpath, setInitpath] = useState("");
    const [selectedNode, setSelectedNode] = useState(null);
    const [selectedNodeCoreValueId, setSelectedNodeCoreValueId] = useState(null);
    const [showAddDialog, setShowAddDialog] = useState(false);
    const [showClearDialog, setShowClearDialog] = useState(false);
    const [showAddReferenceDialog, setShowAddReferenceDialog] = useState(false);
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [parentIdToAdd, setParentIdToAdd] = useState("");
    const [selectNodeActive, setSelectNodeActive] = useState(false);
    const [deleteTarget, setDeleteTarget] = useState(null);
    const [graphId, setGraphId] = useState("");
    const [allConns, setAllConns] = useState(null);

    if(initpath != props.initPath)
    {
        setInitpath(props.initPath);
        if(props.initPath === "/")
        {
            loadGraph("", () => {
                var newPath = [];
                newPath.push({id: "", name: "Root"});
                setPath(newPath);
            });

        }
        else
        {
            resolvePath(props.initPath, result => {
                loadGraph(result[result.length - 1].id, () => {
                    var newPath = [];
                    newPath.push(...result);
                    setPath(newPath);
                });
            });
        }
    }

    function resolvePath(path, cb)
    {
        const splitted = path.split("/");
        var filter = "IS(strategy_node) AND ";
        for(var i = 1; i < splitted.length; ++i)
        {
            if(i != 1)
            {
                filter += " OR ";
            }
            filter += ".title = \"" + splitted[i] + "\"";
        }

        const req = {
            operation: "query",
            filter: filter
        }
        Materia.exec(req, (r) =>
        {
            var resultPath = [{id: "", name: "Root"}]
            if(r.object_list != "")
            {
                for(var j = 1; j < splitted.length; ++j)
                {
                    const val = r.object_list.find(x => x.title === splitted[j]);
                    if(!val)
                    {
                        console.log("Not found " + splitted[j]);
                        break;
                    }

                    resultPath.push({id: val.id, name: val.title});
                }
            }

            cb(resultPath);
        });
    }

    function setNodeStyle(n)
    {
        if(n.orig.isAchieved === 'true')
        {
            n.style.background = "#29c7a7";
        }
        else if(n.orig.hasChildren)
        {
            n.style.background = "#FF4500";
        }
    }

    function buildLinks(conns)
    {
        return conns.All("Requirement").map(r => {
            return {source: r.A, target: r.B, id: r.id, type: "straight", markerEnd: { type: MarkerType.ArrowClosed}};
        });
    }

    function loadGraph(parentId, successCb)
    {
        var filterStr = "";
        if(parentId.length == 0)
        {
            filterStr = "IS(strategy_node) AND RootElement()"
        }
        else
        {
            filterStr = 'IS(strategy_node) AND ChildOf("' + parentId + '")'
        }
        const req = {
            operation: "query",
            filter: filterStr
        };

        Materia.exec(req, (r) =>
        {
            if(r.object_list != "")
            {
                const newNodes = r.object_list.map(x => {
		    return {
			id: x.id,
			position: {x: (x.x ? parseFloat(x.x) : 200), y: (x.y ? parseFloat(x.y) : 200)},
			data: {label: x.title},
		        style: {
		           background: '#444444',
		           color: 'white',
			   width: 75,
			   height: 75
		        },
			className: 'circle',
			sourcePosition: Position.Right,
                        targetPosition: Position.Left,
			orig: x
		    };
                });

                var conns = new MateriaConnections(r.connection_list);
                newNodes.forEach(x => {
                    x.orig.hasChildren = conns.Has(x.id, "ParentOf", "*");
                });

                successCb();
                newNodes.forEach(x => setNodeStyle(x));
		setNodes(newNodes);
		setEdges(buildLinks(conns));
                setAllConns(conns);
                setGraphId(parentId);
                setUpdating(false);
            }
            else
            {
                setUpdating(false);
            }
        });
    }

    const onClickNode = function(ev, n)
    {
	var nodeId = n.id
        if(selectNodeActive)
        {
            setSelectNodeActive(false);
            setUpdating(true);
            var obj = {
                A: selectedNode.id,
                B: nodeId,
                type: "Requirement"
            }
    
            var req = {
                operation: "create",
                typename: "connection",
                params: obj
            }

            Materia.exec(req, (x) => {
                if(x.result_id)
                {
	            setEdges([...edges, {source: selectedNode.id, target: nodeId, id: x.result_id}]);
                }

                setUpdating(false);
            });
        }
        else
        {
            var allRefs = allConns.AllOf(nodeId, "Refers", "*");

            if(allRefs.length > 0)
            {
                //Load all objects
                var loadAllRefs = {
                    operation: "query",
                    ids: allRefs.map(x => x.B)
                }

                Materia.exec(loadAllRefs, (result) => {
                    //Find a reference to core_value among them
                    var cvlist = result.object_list.filter(x => x.typename === "core_value");
                    if(cvlist.length > 0)
                    {
                        setSelectedNodeCoreValueId(cvlist[0].id);
                        setSelectedNode(nodes.find(x => x.id === nodeId).orig);
                    }
                    else
                    {
                        setSelectedNodeCoreValueId(null);
                        setSelectedNode(nodes.find(x => x.id === nodeId).orig);
                    }
                });
            }
            else
            {
                setSelectedNodeCoreValueId(null);
                setSelectedNode(nodes.find(x => x.id === nodeId).orig);
            }
        }
    };

    function fetchNodeName(nodeId)
    {
        if(nodeId === "")
        {
            return "Root";
        }
        return nodes.find(x => x.id === nodeId).orig.title;
    }

    const onDoubleClickNode = function(ev, node)
    {
        setUpdating(true);
        setSelectedNode(null);
        setSelectNodeActive(null);
        var newPath = {id: node.id, name: fetchNodeName(node.id)};
        
        loadGraph(node.id, () => {path.push(newPath);});
    };

    function onPathClick(id)
    {
        var pos = 0;
        for(; pos < path.length; pos++)
        {
            if(path[pos].id === id)
            {
                path.splice(pos + 1);
                setUpdating(true);
                loadGraph(id, () => {});
                break;
            }
        }
    }

    const onNodePositionChange = function(ev, node, nodes)
    {
       var orig = node.orig;
       orig.x = node.position.x;
       orig.y = node.position.y;

       Materia.postEdit(orig.id, JSON.stringify(orig));
    };

    function onAddDialogClosed()
    {
        setShowAddDialog(false);
        setShowAddReferenceDialog(false);
    }

    function onAddDialogOk(title, type, target, timestamp, coreValue)
    {
        setShowAddDialog(false);
        setUpdating(true);

        {
            var obj = {
                title: title,
                typeChoice: type,
                target: target,
                reward: (type === "Goal" || type === "Task" || type === "Counter") ? 1 : 0,
                x: Math.random() * 500,
                y: Math.random() * 500
            };

            if(type === "Wait")
            {
                obj.date = timestamp;
            }
    
            var req = {
                operation: "create",
                typename: "strategy_node",
                params: obj
            }
    
            Materia.exec(req, (x) => {

                if(parentIdToAdd.length > 0)
                {
                    var link = {A: parentIdToAdd, B: x.result_id, type: "Hierarchy"}
                    var linkReq = {
                        operation: "create",
                        typename: "connection",
                        params: link
                    }
                    Materia.exec(linkReq, (u) => {});
                }

                if(coreValue !== "None")
                {
                    if(coreValue === "Parent")
                    {
                        if(parentIdToAdd.length > 0)
                        {
                            //Load all objects
                            var loadAllRefs = {
                                operation: "query",
                                filter: 'ReferedBy("' + parentIdToAdd + '")'
                            }

                            Materia.exec(loadAllRefs, (result) => {
                                //Find a reference to core_value among them
                                var cv = result.object_list.filter(x => x.typename === "core_value")[0];

                                Materia.createConnection(x.result_id, cv.id, "Reference", result_id => {
                                    allConns.Add({id: result_id, A: x.result_id, B: cv.id, type: "Reference"});
                                });
                            });
                        }
                    }
                    else
                    {
                        //Resolve core value case
                        var cv = m3proxy.getType("core_value").objects.filter((x) => x.name === coreValue)[0];

                        Materia.createConnection(x.result_id, cv.id, "Reference", result_id => {
                            allConns.Add({id: result_id, A: x.result_id, B: cv.id, type: "Reference"});
                        });
                    }
                }

                if(graphId == parentIdToAdd)
                {
                    var sreq = {
                        operation: "query",
                        ids: [x.result_id]
                    }
            
                    Materia.exec(sreq, (sresp) => {
                        var x = sresp.object_list[0];
                        var n = {
				id: x.id,
				position: {x: (x.x ? parseFloat(x.x) : 200), y: (x.y ? parseFloat(x.y) : 200)},
				data: {label: x.title},
				style: {
				   background: '#444444',
				   color: 'white',
				   width: 75,
				   height: 75
				},
				className: 'circle',
				sourcePosition: Position.Right,
				targetPosition: Position.Left,
				orig: x
			    };
			setNodeStyle(n);
			setNodes([...nodes, n]);
    
                        setUpdating(false);
                    });
                }
                else
                {
                    selectedNode.hasChildren = true;
                    onNodeChanged(selectedNode);
                    setUpdating(false);
                }
            });
        }
    }

    function onAddClicked()
    {
        setShowAddDialog(true);
        setParentIdToAdd(graphId);
    }

    function onClearClicked()
    {
        setShowClearDialog(true);
    }

    function onNodeAddClicked()
    {
        setShowAddDialog(true);
        setParentIdToAdd(selectedNode.id);
    }

    function onNodeDelete()
    {
        setInDeleteDialog(true);
        setDeleteTarget(selectedNode);
    }

    function onDeleteDialogCancel()
    {
        setInDeleteDialog(false);
    }

    function onDeleteDialogOk()
    {
        if(deleteTarget.target)//link
        {
            Materia.postDelete(deleteTarget.id);
        }
        else //node
        {
            Materia.postDelete(selectedNode.id);
            setSelectedNode(null);
        }
        setUpdating(true);
        loadGraph(path[path.length - 1].id, () => {});
        setDeleteTarget(null);
        setInDeleteDialog(false);
    }

    function onAddLinkClicked()
    {
        setSelectNodeActive(true);
    }

    function onAddCalendarClicked()
    {
        setShowAddReferenceDialog(true);
    }

    function onCancelClicked()
    {
        setSelectNodeActive(false);
    }

    const onClickLink = function(ev, edge)
    {
        var l = edges.find(x => x.source === edge.source && x.target === edge.target);
        setDeleteTarget(l);
        setInDeleteDialog(true);
    };

    function onNodeChanged(n, newCoreValue)
    {
	var graphNode = nodes.find(x => x.id === n.id);
	graphNode.orig = {...n};
        Materia.postEdit(n.id, JSON.stringify(n));

        if(newCoreValue !== resolveCoreValueId(selectedNodeCoreValueId))
        {
            if(selectedNodeCoreValueId)
            {
                //Remove previous connection
                var ref = allConns.AllOf(n.id, "Refers", selectedNodeCoreValueId)[0];
                allConns.Remove(ref.id);

                Materia.postDelete(ref.id);
            }
            if(newCoreValue !== "None")
            {
                //Connect new connection
                var cv = m3proxy.getType("core_value").objects.filter((x) => x.name === newCoreValue)[0];

                Materia.createConnection(n.id, cv.id, "Reference", result_id => {
                    allConns.Add({id: result_id, A: n.id, B: cv.id, type: "Reference"});
                });
            }
        }

	setNodes(nodes.map(x => {
	   if(x.id === n.id)
	   {
	       var newNode = {...x};
	       newNode.data = {label: n.title};
	       return newNode;
	   }
	   return x;
	}));
    }

    function onNodeCompleted()
    {
        Materia.sendComplete(selectedNode.id, (resp) => {
            setSelectedNode(null);

            setUpdating(true);
            loadGraph(path[path.length - 1].id, () => {});
        });
    }

    function onWatchCompleted(id)
    {
        var n = nodes.find(x => x.id === id).orig;
        n.isAchieved = true;
        Materia.postEdit(n.id, JSON.stringify(n));

        setUpdating(true);
        loadGraph(path[path.length - 1].id, () => {});
    }

    function createLinks(links)
    {
        var pos = 0;
        for(; pos < links.length; ++pos)
        {
            var req = {
                operation: "create",
                typename: "connection",
                params: {A: links[pos].A, B: links[pos].B, type: "Requirement"}
            }

            Materia.exec(req, (u) => {});
        }
        
        loadGraph(path[path.length - 1].id, () => {});
    }

    function onNodeSplitHorizClicked()
    {
        setUpdating(true);

        var req = {
            operation: "create",
            typename: "strategy_node",
            params: selectedNode
        }

        req.params.x += 100;

        Materia.exec(req, (res) => {

            var newId = res.result_id;
            Materia.createConnection(newId, selectedNodeCoreValueId, "Reference", result_id => {
                allConns.Add({id: result_id, A: newId, B: selectedNodeCoreValueId, type: "Reference"});
            });

            if(graphId.length > 0)
            {
                var link = {A: graphId, B: res.result_id, type: "Hierarchy"}
                var linkReq = {
                    operation: "create",
                    typename: "connection",
                    params: link
                }
                Materia.exec(linkReq, (u) => {});
            }

            //Remove all outgoing links
            //Connect to new node
            //Connect new node with removed links
            var links = edges.filter(x => x.source === selectedNode.id).map(y => {return {A: newId, B: y.target}});
            links.push({A: selectedNode.id, B: newId});

            var toRemove = edges.filter(x => x.source === selectedNode.id);
            toRemove.forEach(x => Materia.postDelete(x.id));

            createLinks(links);

        });
    }

    function onNodeSplitVertClicked()
    {
        setUpdating(true);

        var req = {
            operation: "create",
            typename: "strategy_node",
            params: selectedNode
        }

        req.params.y += 100;

        Materia.exec(req, (res) => {
            var newId = res.result_id;
            Materia.createConnection(newId, selectedNodeCoreValueId, "Reference", result_id => {
                allConns.Add({id: result_id, A: newId, B: selectedNodeCoreValueId, type: "Reference"});
            });

            if(graphId.length > 0)
            {
                var link = {A: graphId, B: res.result_id, type: "Hierarchy"}
                var linkReq = {
                    operation: "create",
                    typename: "connection",
                    params: link
                }
                Materia.exec(linkReq, (u) => {});
            }
            //Make same set of links for new node
            var inLinks = edges.filter(x => x.target === selectedNode.id).map(y => {return {B: newId, A: y.source}});
            var outLinks = edges.filter(x => x.source === selectedNode.id).map(y => {return {B: y.target, A: newId}});
            var links = inLinks.concat(outLinks);

            createLinks(links);

        });
    }

    function onClearDialogCancel()
    {
        setShowClearDialog(false);
    }

    function onCalendarReferenceCreated(id)
    {
        var link = {A: id, B: selectedNode.id, type: "Reference"}
        var linkReq = {
            operation: "create",
            typename: "connection",
            params: link
        }
        Materia.exec(linkReq, (u) => {});
    }

    function onClearDialogOk()
    {
        setShowClearDialog(false);

        setUpdating(true);

        var toDelete = nodes.filter(x => x.orig.isAchieved === 'true');
	
	const chainDelete = (arr) => {
            if (arr.length > 0)
	    {
	        Materia.sendDelete(arr[0].id, x => {chainDelete(arr.slice(1))});
	    }
	    else
	    {
		loadGraph(path[path.length - 1].id, () => {});
            }
	};

        chainDelete(toDelete);
    }

    function resolveCoreValueId(id)
    {
        if(!id)
        {
            return "None";
        }
        else
        {
            return m3proxy.getType("core_value").objects.filter((x) => x.id === id)[0].name;
        }
    }

    return (
        <div>
        <Backdrop open={updating}><CircularProgress color="inherit"/></Backdrop>
        {showAddDialog && <AddNodeDialog onClose={onAddDialogClosed} onOk={onAddDialogOk}/>}
        {showAddReferenceDialog && <AddItemDialog onClose={onAddDialogClosed} onCreated={onCalendarReferenceCreated} selectedType="calendar_item" init={{entityTypeChoice: "StrategyNodeReference", text: selectedNode.title}}/>}
        <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
        <ConfirmationDialog open={showClearDialog} question="clear it" caption="confirm clear" onNo={onClearDialogCancel} onYes={onClearDialogOk} />
        <Grid container direction="row" justify="space-around" alignItems="flex-start">
            <div style={{width: '75vw', height: '75vh', paddingLeft: 10}}>
                <Grid container direction="row" justify="flex-start" alignItems="center">
                    <IconButton edge="start" aria-label="complete" onClick={onAddClicked}>
                        <AddCircleOutlineIcon/>
                    </IconButton>
                    <IconButton edge="start" aria-label="clear" onClick={onClearClicked}>
                        <ClearAllIcon/>
                    </IconButton>
                    {path.length > 0 && <PathCtrl currentText={path[path.length - 1].name} pathList={path.slice(0, path.length - 1)} onClick={onPathClick}/>}
                </Grid>
                {nodes && edges && <WatchPanel nodes={nodes.filter(x => x.typeChoice === "Watch" && x.isAchieved==='false')} completed={onWatchCompleted}/>}
		<div style={{width: '75vw', height: '75vh'}}>
			{nodes && edges && <ReactFlow
			    nodes={nodes}
			    edges={edges}
			    fitView
			    onNodeClick={onClickNode}
			    onNodesChange={onNodesChange}
                            onEdgesChange={onEdgesChange}
			    onNodeDoubleClick={onDoubleClickNode}
			    onNodeDragStop={onNodePositionChange}
			    onEdgeClick={onClickLink}
			>
				<Controls/>
				<Background/>
			</ReactFlow>
			}
	        </div>
            </div>
            <div style={{width: '25vw'}}>
                {selectNodeActive && <Grid container direction="column" justify="flex-start" alignItems="center">
                    <Typography variant="h4">Select node to link</Typography>
                    <Button variant="contained" color="primary" onClick={onCancelClicked}>Cancel</Button>
                </Grid>}
                {!selectNodeActive && selectedNode && <NodeInfoView
                    node={selectedNode}
                    coreValue={resolveCoreValueId(selectedNodeCoreValueId)}
                    deleteClicked={onNodeDelete}
                    addClicked={onNodeAddClicked}
                    addLinkClicked={onAddLinkClicked}
                    addCalendarClicked={onAddCalendarClicked}
                    onChanged={onNodeChanged}
                    onCompleted={onNodeCompleted}
                    onSplitHorizClicked={onNodeSplitHorizClicked}
                    onSplitVertClicked={onNodeSplitVertClicked}
                    />}
            </div>
        </Grid>
        </div>
    );
}

export default StrategyView;

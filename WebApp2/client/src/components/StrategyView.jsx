import React, { useState } from 'react';
import Materia from '../modules/materia_request'
import { Graph } from "react-d3-graph";
import PathCtrl from './PathCtrl.jsx'
import m3proxy from '../modules/m3proxy'
import QueryChain from '../modules/QueryChain'

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
        props.onChanged(props.node);
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
            {(node.typeChoice === "Goal" || node.typeChoice === "Task") && !isAchieved && <IconButton edge="end" aria-label="complete" onClick={completeClicked}>
                <DoneIcon/>
            </IconButton>}
            <Divider/>
            <Grid container direction="column" justify="flex-start" alignItems="center">
                <Typography color={isAchieved ? "primary" : "white"} variant="h6">{isAchieved ? "Achieved" : "In Progress"}</Typography>
            </Grid>
            <TextField inputProps={{onChange: handleTitleChange}} value={title} fullWidth id="title" label="Title" />
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

function AddNodeDialog(props)
{
    const [title, setTitle] = useState("");
    const [type, setType] = useState("Task");
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
            </DialogContent>
            <DialogActions>
                <Button onClick={props.onClose} variant="contained" color="primary">
                    Cancel
                </Button>
                <Button onClick={() => props.onOk(title, type, target, date)} variant="contained" color="primary" autoFocus>
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
                    {x.title}
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
    const [graphData, setGraphData] = useState(null);
    const [path, setPath] = useState([]);
    const [selectedNode, setSelectedNode] = useState(null);
    const [showAddDialog, setShowAddDialog] = useState(false);
    const [showClearDialog, setShowClearDialog] = useState(false);
    const [showAddReferenceDialog, setShowAddReferenceDialog] = useState(false);
    const [inDeleteDialog, setInDeleteDialog] = useState(false);
    const [parentIdToAdd, setParentIdToAdd] = useState("");
    const [selectNodeActive, setSelectNodeActive] = useState(false);
    const [deleteTarget, setDeleteTarget] = useState(null);
    const [showCompleted, setShowCompleted] = useState(true);
    const [showLocked, setShowLocked] = useState(true);

    if(graphData == null)
    {
        loadGraph("", () => {path.push({id: "", name: "Root"});});
    }

    function isNodeLocked(n)
    {
        var locked = false;
        //Node is locked if at least one predecessor is not completed
        var predecessors = graphData.links.filter(x => x.target === n.id).map(x =>  {return x.source});
        
        predecessors.forEach(x =>
        {
            var p = graphData.nodes.find(y => x === y.id);
            console.log(p);
            if(!(p.isAchieved === "true"))
            {
                locked = true;
            }
        });

        return locked;
    }

    function setNodeStyle(n, hideCompleted, hideLocked)
    {
        n.opacity = 1;

        if(n.isAchieved === 'true')
        {
            n.color = "#29c7a7";
            n.fontColor = "#29c7a7"
            if(hideCompleted)
            {
                n.opacity = 0.1;
            }
        }
        else if(n.childrenCount > 0)
        {
            n.color = "#FF4500";
        }

        if(hideLocked && isNodeLocked(n))
        {
            n.opacity = 0.1;
        }
    }

    function loadGraph(parentId, successCb)
    {
        const req = {
            operation: "query",
            filter: "IS(strategy_node) AND .parentNodeId = \"" + parentId + '\"'
        };

        Materia.exec(req, (r) => 
        {
            if(r.object_list != "")
            {
                const newNodes = r.object_list.map(x => {
                    var n = x;
                    if(x.x)
                    {
                        n.x = parseFloat(x.x);
                        n.y = parseFloat(x.y);
                    }
                    else
                    {
                        n.x = Math.random() * 500;
                        n.y = Math.random() * 500;
                    }

                    return n;
                });

                var newGraphData = {nodes: newNodes, links: []};

                var reqs = [];
                newNodes.forEach(x => {

                    const linkReq = {
                        operation: "query",
                        filter: "IS(strategy_link) AND .idFrom = \"" + x.id + "\""
                    }

                    const step = (rsp) => {
                        if(rsp.object_list != "")
                        {
                            const newLinks = rsp.object_list.map(y => {
                                return {source: y.idFrom, target: y.idTo, id: y.id};
                            });
            
                            newGraphData.links = newGraphData.links.concat(newLinks);
                        }
                    }

                    reqs.push({req: linkReq, step: step});

                    const childrenReq = {
                        operation: "query",
                        filter: "IS(strategy_node) AND .parentNodeId = \"" + x.id + '\"'
                    }

                    const childrenStep = (rsp) => {
                        if(rsp.object_list != "")
                        {
                            x.childrenCount = rsp.object_list.length;
                        }
                        else
                        {
                            x.childrenCount = 0;
                        }
                    }

                    reqs.push({req: childrenReq, step: childrenStep});
                });

                var done = () => {
                    successCb();
                    newGraphData.nodes.forEach(x => setNodeStyle(x, !showCompleted, !showLocked));
                    updateLinkStyling(newGraphData);
                    setGraphData(newGraphData);
                    setUpdating(false);
                }

                QueryChain(reqs, done);
            }
            else
            {
                setUpdating(false);
            }
        });
    }

    const myConfig = {
        "height": "85vh",
        "maxZoom": 8,
        "minZoom": 0.1,
        "width": "75vw",
        "directed": true,
        "nodeHighlightBehavior": false,
        "d3": {
          "alphaTarget": 0.05,
          "gravity": -100,
          "linkLength": 500,
          "linkStrength": 1,
          "disableLinkForce": true
        },
        "node": {
          "color": "#d3d3d3",
          "fontColor": "white",
          "fontSize": 12,
          "fontWeight": "normal",
          "highlightColor": "green",
          "highlightFontSize": 12,
          "highlightFontWeight": "normal",
          "highlightStrokeColor": "SAME",
          "highlightStrokeWidth": "SAME",
          "labelProperty": "title",
          "labelPosition": "bottom",
          "mouseCursor": "pointer",
          "opacity": 1,
          "renderLabel": true,
          "size": 1000,
          "strokeColor": "red",
          "strokeWidth": 0,
          "svg": "",
          "symbolType": "circle"
        },
        "link": {
          "color": "#d3d3d3",
          "fontColor": "black",
          "fontSize": 8,
          "fontWeight": "normal",
          "highlightColor": 'lightblue',
          "highlightFontSize": 8,
          "highlightFontWeight": "normal",
          "labelProperty": "label",
          "mouseCursor": "pointer",
          "opacity": 1,
          "renderLabel": false,
          "semanticStrokeWidth": false,
          "strokeWidth": 1.5,
          "markerHeight": 6,
          "markerWidth": 6
        }
    };

    const onClickNode = function(nodeId) 
    {
        if(selectNodeActive)
        {
            setSelectNodeActive(false);
            setUpdating(true);
            var obj = {
                idFrom: selectedNode.id,
                idTo: nodeId
            }
    
            var req = {
                operation: "create",
                typename: "strategy_link",
                params: obj
            }
    
            Materia.exec(req, (x) => {
                if(x.result_id)
                {
                    graphData.links.push({source: selectedNode.id, target: nodeId, id: x.result_id});
                }

                setUpdating(false);
            });
        }
        else
        {
            setSelectedNode(graphData.nodes.find(x => x.id === nodeId));
        }
    };

    function fetchNodeName(nodeId)
    {
        if(nodeId === "")
        {
            return "Root";
        }
        return graphData.nodes.find(x => x.id === nodeId).title;
    }

    const onDoubleClickNode = function(nodeId) 
    {
        setUpdating(true);
        setSelectedNode(null);
        setSelectNodeActive(null);
        var newPath = {id: nodeId, name: fetchNodeName(nodeId)};
        
        loadGraph(nodeId, () => {path.push(newPath);});
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

    const onNodePositionChange = function(nodeId, x, y) 
    {
       var n = graphData.nodes.find(x => x.id === nodeId);
       n.x = x;
       n.y = y;

       Materia.postEdit(nodeId, JSON.stringify(n));
    };

    function onAddDialogClosed()
    {
        setShowAddDialog(false);
        setShowAddReferenceDialog(false);
    }

    function updateLinkStyling(gd)
    {
        gd.links.forEach(l => {
            var n1 = gd.nodes.find(x => x.id === l.source);
            var n2 = gd.nodes.find(x => x.id === l.target);

            l.opacity = Math.min(n1.opacity, n2.opacity);
        });
    }

    function onAddDialogOk(title, type, target, timestamp)
    {
        setShowAddDialog(false);
        setUpdating(true);

        {
            var obj = {
                title: title,
                typeChoice: type,
                parentNodeId: parentIdToAdd,
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

                if(graphData.nodes[0].parentNodeId == parentIdToAdd)
                {
                    var sreq = {
                        operation: "query",
                        ids: [x.result_id]
                    }
            
                    Materia.exec(sreq, (sresp) => {
                        var x = sresp.object_list[0];
                        var n = x;
                        n.x = parseFloat(x.x);
                        n.y = parseFloat(x.y);
                        graphData.nodes.push(n);
    
                        setUpdating(false);
                    });
                }
                else
                {
                    selectedNode.childrenCount++;
                    onNodeChanged(selectedNode);
                    setUpdating(false);
                }
            });
        }
    }

    function onAddClicked()
    {
        setShowAddDialog(true);
        setParentIdToAdd(graphData.nodes[0].parentNodeId);
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

    const onClickLink = function(source, target) 
    {
        var l = graphData.links.find(x => x.source === source && x.target === target);
        setDeleteTarget(l);
        setInDeleteDialog(true);
    };

    function onNodeChanged(n)
    {
        graphData.nodes[graphData.nodes.indexOf(graphData.nodes.find(x => x.id === n.id))] = n;
        Materia.postEdit(n.id, JSON.stringify(n));

        setNodeStyle(n, !showCompleted, !showLocked);
        updateLinkStyling(graphData);
        setGraphData(JSON.parse(JSON.stringify(graphData)));
    }

    function onNodeCompleted()
    {
        selectedNode.isAchieved = true;
        Materia.postEdit(selectedNode.id, JSON.stringify(selectedNode));

        setSelectedNode(null);

        setUpdating(true);
        loadGraph(path[path.length - 1].id, () => {});
    }

    function onWatchCompleted(id)
    {
        var n = graphData.nodes.find(x => x.id === id);
        n.isAchieved = true;
        Materia.postEdit(n.id, JSON.stringify(n));

        setUpdating(true);
        loadGraph(path[path.length - 1].id, () => {});
    }

    function createLinks(links)
    {
        console.log(links);
        var pos = 0;
        for(; pos < links.length; ++pos)
        {
            var req = {
                operation: "create",
                typename: "strategy_link",
                params: links[pos]
            }

            Materia.post(req);
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
            //Remove all outgoing links
            //Connect to new node
            //Connect new node with removed links
            var links = graphData.links.filter(x => x.source === selectedNode.id).map(y => {return {idFrom: newId, idTo: y.target}});
            links.push({idFrom: selectedNode.id, idTo: newId});

            var toRemove = graphData.links.filter(x => x.source === selectedNode.id);
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
            //Make same set of links for new node
            var inLinks = graphData.links.filter(x => x.target === selectedNode.id).map(y => {return {idTo: newId, idFrom: y.source}});
            var outLinks = graphData.links.filter(x => x.source === selectedNode.id).map(y => {return {idTo: y.target, idFrom: newId}});
            var links = inLinks.concat(outLinks); 

            createLinks(links);
        });
    }

    function handleShowCompletedChange(e)
    {
        setShowCompleted(e.target.checked);

        graphData.nodes.forEach(x => setNodeStyle(x, !e.target.checked, !showLocked));

        updateLinkStyling(graphData);

        setGraphData(JSON.parse(JSON.stringify(graphData)));
    }

    function handleShowLockedChange(e)
    {
        setShowLocked(e.target.checked);

        graphData.nodes.forEach(x => setNodeStyle(x, !showCompleted, !e.target.checked));

        updateLinkStyling(graphData);

        setGraphData(JSON.parse(JSON.stringify(graphData)));
    }
    
    function onClearDialogCancel()
    {
        setShowClearDialog(false);
    }

    function sleep(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    function onClearDialogOk()
    {
        setShowClearDialog(false);

        setUpdating(true);

        graphData.nodes.filter(x => x.isAchieved === 'true').forEach(x => Materia.postDelete(x.id));

        sleep(2000);

        loadGraph(path[path.length - 1].id, () => {});
    }

    return (
        <div>
        <Backdrop open={updating}><CircularProgress color="inherit"/></Backdrop>
        {showAddDialog && <AddNodeDialog onClose={onAddDialogClosed} onOk={onAddDialogOk}/>}
        {showAddReferenceDialog && <AddItemDialog onClose={onAddDialogClosed} selectedType="calendar_item" init={{entityTypeChoice: "StrategyNodeReference", text: selectedNode.title, nodeReference: selectedNode.id}}/>}
        <ConfirmationDialog open={inDeleteDialog} question="delete object" caption="confirm deletion" onNo={onDeleteDialogCancel} onYes={onDeleteDialogOk} />
        <ConfirmationDialog open={showClearDialog} question="clear it" caption="confirm clear" onNo={onClearDialogCancel} onYes={onClearDialogOk} />
        <Grid container direction="row" justify="space-around" alignItems="flex-start">
            <div style={{width: '75vw'}}>
                <FormControlLabel margin='dense' control={<Checkbox onChange={handleShowCompletedChange} checked={showCompleted} />} label="Show Completed" />
                <FormControlLabel margin='dense' control={<Checkbox onChange={handleShowLockedChange} checked={showLocked} />} label="Show Locked" />
                <Grid container direction="row" justify="flex-start" alignItems="center">
                    <IconButton edge="start" aria-label="complete" onClick={onAddClicked}>
                        <AddCircleOutlineIcon/>
                    </IconButton>
                    <IconButton edge="start" aria-label="clear" onClick={onClearClicked}>
                        <ClearAllIcon/>
                    </IconButton>
                    {path.length > 0 && <PathCtrl currentText={path[path.length - 1].name} pathList={path.slice(0, path.length - 1)} onClick={onPathClick}/>}
                </Grid>
                {graphData && <WatchPanel nodes={graphData.nodes.filter(x => x.typeChoice === "Watch" && x.isAchieved==='false')} completed={onWatchCompleted}/>}
                {graphData && <Graph
                    id="graph-id"
                    data={graphData}
                    onClickNode={onClickNode}
                    onDoubleClickNode={onDoubleClickNode}
                    onNodePositionChange={onNodePositionChange}
                    onClickLink={onClickLink}
                    config={myConfig}/>}
            </div>
            <div style={{width: '20vw'}}>
                {selectNodeActive && <Grid container direction="column" justify="flex-start" alignItems="center">
                    <Typography variant="h4">Select node to link</Typography>
                    <Button variant="contained" color="primary" onClick={onCancelClicked}>Cancel</Button>
                </Grid>}
                {!selectNodeActive && selectedNode && <NodeInfoView 
                    node={selectedNode} 
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
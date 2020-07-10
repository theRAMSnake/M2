import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'

import {
    Typography,
    Table,
    TableRow,
    TableCell,
    TableHead,
    TableBody,
    Grid,
    TableContainer,
    Paper
} from "@material-ui/core";

function getHeaderCell(key)
{
    if(key === 'id' || key === 'typename')
    {
        return (<div/>);
    }

    return <TableCell>
        {key}
    </TableCell>
}

function getHeaderRow(report)
{
    return <TableRow hover>
        <TableCell>Category</TableCell>
        {Object.keys(report).map((key) => getHeaderCell(key))}
        <TableCell>Total</TableCell>
    </TableRow>
}

function getCell(field, obj)
{
    if(field === 'id' || field === 'typename')
    {
        return (<div/>);
    }

    return <TableCell>
        {obj[field] && obj[field] / 100}
    </TableCell>
}

function getCategoryRow(field, value, keys)
{
    if(field === 'id' || field === 'status' || field === 'typename' || field === 'balance' || field === 'totalPerMonth')
    {
        return (<div/>);
    }

    return <TableRow hover>
        <TableCell>{field}</TableCell>
        {keys.map((obj) => getCell(obj, value))}
        {!(field === "Total") && <TableCell>{value.total / 100}</TableCell>}
    </TableRow>
}

function getReportTable(report)
{
    return (<TableContainer component={Paper}>
        <Table size="small">
            <TableHead>
                {getHeaderRow(report.totalPerMonth)}
            </TableHead>
            <TableBody>
                {Object.keys(report).map((obj) => getCategoryRow(obj, report[obj], Object.keys(report.totalPerMonth)))}
                {getCategoryRow("Total", report.totalPerMonth, Object.keys(report.totalPerMonth))}
            </TableBody>
        </Table>
    </TableContainer>);
}

function FinanceView(props) 
{
    const [report, setReport] = useState(null);

    if(report == null)
    {
        const req = {
            operation: "query",
            ids: ["financial_report"]
        };

        MateriaRequest.req(JSON.stringify(req), (r) => {
            setReport(JSON.parse(r).object_list[0]);
        });
    }

    return (
        <div>{report && ( 
            <Grid  style={{paddingTop:'5px'}} container direction="column" justify="center" alignItems="center">
                <Typography variant="h6" color={report.status === "Critical" ? "red" : "primary"}>
                    Status: {report.status}, Balance: {report.balance / 100}€
                </Typography>
            </Grid>)}
            {report && getReportTable(report)}
        </div>
    );
}

export default FinanceView;
import React, { useState } from 'react';
import Materia from '../modules/materia_request'

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
        {obj[field] && parseFloat(obj[field])}
    </TableCell>
}

function getCategoryRow(field, value, keys)
{
    if(field === 'id' || field === 'status' || field === 'typename' || field === 'balance' || field === 'totalPerMonth' || field === 'modified')
    {
        return (<div/>);
    }

    return <TableRow hover>
        <TableCell>{field}</TableCell>
        {keys.map((obj) => getCell(obj, value))}
        {!(field === "Total") && <TableCell>{parseFloat(value.total)}</TableCell>}
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

        Materia.exec(req, (r) => {
            setReport(r.object_list[0]);
        });
    }

    return (
        <div>{report && ( 
            <Grid  style={{paddingTop:'5px'}} container direction="column" justify="center" alignItems="center">
                <Typography variant="h6" color={report.status === "Critical" ? "red" : "primary"}>
                    Status: {report.status}, Balance: {parseFloat(report.balance)}€
                </Typography>
            </Grid>)}
            {report && getReportTable(report)}
        </div>
    );
}

export default FinanceView;

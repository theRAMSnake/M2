import React, { useState } from 'react';
import MateriaRequest from '../modules/materia_request'

import {
    Typography,
    Grid
} from "@material-ui/core";

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
        <div>{report && 
            <Grid  style={{paddingTop:'5px'}} container direction="column" justify="center" alignItems="center">
                <Typography variant="h6" color={report.status === "Critical" ? "red" : "primary"}>
                    Status: {report.status}, Balance: {report.balance / 100}€
                </Typography>
            </Grid>
            }
        </div>
    );
}

export default FinanceView;
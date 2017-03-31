#!/bin/bash
pkill m2InboxService
pkill m2ActionsService
./m2InboxService &
./m2ActionsService &
./m2central
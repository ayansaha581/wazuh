# Copyright (C) 2015, Wazuh Inc.
# Created by Wazuh, Inc. <info@wazuh.com>.
# This program is a free software; you can redistribute it and/or modify it under the terms of GPLv2

import logging
from typing import Optional

from wazuh.core.cluster.dapi.dapi import DistributedAPI
from wazuh.engine import metrics
from api.util import raise_if_exc

logger = logging.getLogger('wazuh-api')

# TODO Define the max number of limit
HARDCODED_VALUE_TO_SPECIFY = 10000


async def get_metrics(request, scope_name: Optional[str] = None,  instrument_name: Optional[str] = None,
                      select: Optional[str] = None, sort: Optional[str] = None, search: Optional[str] = None,
                      offset: int = 0, limit: int = HARDCODED_VALUE_TO_SPECIFY):
    """Get a single metric or all the collected metrics. Uses the metrics/get and metrics/dump actions
    of the engine.

    Parameters
    ----------
    request : connexion.request
    scope_name: Optional[str]
        Name of the metric scope.  If it is None and the instrument_name is None, returns all
        metrics.
    instrument_name: Optional[str]
        Name of the metric instrument. If it is None and the scope_name is None, returns all
        metrics.
    select : str
        Select which fields to return (separated by comma).
    sort : str
        Sort the collection by a field or fields (separated by comma). Use +/- at the beginning to list in
        ascending or descending order.
    search : str
        Look for elements with the specified string.
    offset : int
        First element to return in the collection.
    limit : int
        Maximum number of elements to return. Default: HARDCODED_VALUE_TO_SPECIFY


    Returns
    -------
    TODO
    """

    f_kwargs = {
        'scope_name': scope_name,
        'instrument_name': instrument_name,
        'select': select,
        'sort': sort,
        'search': search,
        'offset': offset,
        'limit': limit
    }

    dapi = DistributedAPI(f=metrics.get_metrics,
                          f_kwargs=f_kwargs,
                          request_type='local_master',
                          is_async=False,
                          logger=logger,
                          rbac_permissions=request['token_info']['rbac_policies']
                          )

    data = raise_if_exc(await dapi.distribute_function())

    return


async def get_instruments(request, select: Optional[str] = None, sort: Optional[str] = None,
                          search: Optional[str] = None, offset: int = 0, limit: int = HARDCODED_VALUE_TO_SPECIFY):
    """Get all name, status and instruments types. Uses the metrics/list action
    of the engine.

    Parameters
    ----------
    request : connexion.request
    select : Optional[str]
        Select which fields to return (separated by comma).
    sort :Optional[str]
        Sort the collection by a field or fields (separated by comma). Use +/- at the beginning to list in
        ascending or descending order.
    search : Optional[str]
        Look for elements with the specified string.
    offset : int
        First element to return in the collection.
    limit : int
        Maximum number of elements to return. Default: HARDCODED_VALUE_TO_SPECIFY


    Returns
    -------
    TODO
    """
    f_kwargs = {
        'select': select,
        'sort': sort,
        'search': search,
        'offset': offset,
        'limit': limit
    }

    dapi = DistributedAPI(f=metrics.get_instruments,
                          f_kwargs=f_kwargs,
                          request_type='local_master',
                          is_async=False,
                          logger=logger,
                          rbac_permissions=request['token_info']['rbac_policies']
                          )

    data = raise_if_exc(await dapi.distribute_function())

    return


async def enable_instrument(request, scope_name: Optional[str] = None, instrument_name: Optional[str] = None,
                        enable: bool = True):
    """Enable or disable a specified metric. Uses the metrics/enable action
    of the engine.

    Parameters
    ----------
    request : connexion.request
    scope_name: Optional[str]
        Name of the metric scope.  If it is None and the instrument_name is None, returns all
        metrics.
    instrument_name: Optional[str]
        Name of the metric instrument. If it is None and the scope_name is None, returns all
        metrics.
    enable: bool
        Enable of disable the metric. True represent enable, false disable.

    Returns
    -------
    TODO
    """
    f_kwargs = {
        'scope_name': scope_name,
        'instrument_name': instrument_name,
        'enable': enable
    }

    dapi = DistributedAPI(f=metrics.enable_instrument,
                          f_kwargs=f_kwargs,
                          request_type='local_master',
                          is_async=False,
                          logger=logger,
                          rbac_permissions=request['token_info']['rbac_policies']
                          )

    data = raise_if_exc(await dapi.distribute_function())

    return


async def get_test_dummy_metric(request):
    """Generate dummy metrics for testing.

    Parameters
    ----------
    request : connexion.request

    Returns
    -------
    TODO
    """
    dapi = DistributedAPI(f=metrics.get_test_dummy_metric,
                          f_kwargs={},
                          request_type='local_master',
                          is_async=False,
                          logger=logger,
                          rbac_permissions=request['token_info']['rbac_policies']
                          )

    data = raise_if_exc(await dapi.distribute_function())

    return

/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */
/**
 * Integrating a DataAccess example VarieableType with Properties
 * DataAccess defines new Variable Types and Properties for describing real time data.
 * In this example a VariableType node is added with its respective DataAccess Properties
*/
#include "open62541.h"
#include <signal.h>
#include "ua_server_dataaccess.h"

UA_Boolean running = true;
static void stopHandler(int sig) {
    running = false;
}

static void
setVariableAttributes(UA_VariableAttributes* variable_attr, UA_LocalizedText text, UA_Int32 vRank, UA_DataType type)
{
    *variable_attr = UA_VariableAttributes_default; //resets the variable attribute
    variable_attr->displayName = text;
    variable_attr->valueRank = vRank;
    variable_attr->accessLevel = UA_ACCESSLEVELMASK_WRITE| UA_ACCESSLEVELMASK_READ;
    variable_attr->userAccessLevel = UA_ACCESSLEVELMASK_WRITE| UA_ACCESSLEVELMASK_READ;
    variable_attr->dataType = UA_NODEID_NUMERIC(0, type.typeId.identifier.numeric);
}

static UA_StatusCode
addDataAccessNode(UA_Server *server)
{
    UA_VariableAttributes variable_attr = UA_VariableAttributes_default;
    UA_ObjectAttributes object_attr = UA_ObjectAttributes_default;
    UA_StatusCode retval = UA_STATUSCODE_GOOD;

    object_attr.displayName = UA_LOCALIZEDTEXT("", "DataAccess Example");
    retval |= UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(0, 999999), UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), UA_QUALIFIEDNAME(0, "DataAccessCTT"),
                                      UA_NODEID_NUMERIC(0, UA_NS0ID_FOLDERTYPE), object_attr, NULL, NULL);

    setVariableAttributes(&variable_attr,
                          UA_LOCALIZEDTEXT_ALLOC("", (const char*)UA_TYPES[UA_TYPES_INT16].typeName),
                          UA_VALUERANK_SCALAR, UA_TYPES[UA_TYPES_INT16]);
    void *value = UA_new(&UA_TYPES[UA_TYPES_INT16]);
    UA_Variant_setScalar(&variable_attr.value, value, &UA_TYPES[UA_TYPES_INT16]);
    UA_QualifiedName browseName = UA_QUALIFIEDNAME_ALLOC(0, UA_TYPES[UA_TYPES_INT16].typeName);
    retval |= UA_Server_addNode_begin(server, UA_NODECLASS_VARIABLE, UA_NODEID_NUMERIC(0, 500111),
                                      UA_NODEID_NUMERIC(0, 999999), UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                      browseName, UA_NODEID_NUMERIC(0, UA_NS0ID_ANALOGITEMTYPE),
                                      (const UA_NodeAttributes*)&variable_attr, &UA_TYPES[UA_TYPES_VARIABLEATTRIBUTES], NULL, NULL);

    setVariableAttributes(&variable_attr, UA_LOCALIZEDTEXT("", "EURange"), UA_VALUERANK_SCALAR, UA_TYPES[UA_TYPES_RANGE]);
    UA_Range euRange = {0, 200};
    UA_Variant_setScalar(&variable_attr.value, &euRange, &UA_TYPES[UA_TYPES_RANGE]);
    retval |= UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(0, 5011111), UA_NODEID_NUMERIC(0, 500111),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(0, "EURange"),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), variable_attr, NULL, NULL);

    setVariableAttributes(&variable_attr, UA_LOCALIZEDTEXT("", "EngineeringUnits"), UA_VALUERANK_SCALAR, UA_TYPES[UA_TYPES_EUINFORMATION]);
    UA_EUInformation engineeringUnit = {UA_STRING("www.CTTtesting.com"), UA_DA_UNITID_NEWTON_METRE,
                                        UA_LOCALIZEDTEXT("", "Nm"), UA_LOCALIZEDTEXT("", UA_DA_UNITID_NEWTON_METRE_NAME)};
    UA_Variant_setScalar(&variable_attr.value, &engineeringUnit, &UA_TYPES[UA_TYPES_EUINFORMATION]);
    retval |= UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(0, 5021111), UA_NODEID_NUMERIC(0, 500111),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(0, "EngineeringUnits"),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), variable_attr, NULL, NULL);

    setVariableAttributes(&variable_attr, UA_LOCALIZEDTEXT("", "InstrumentRange"), UA_VALUERANK_SCALAR, UA_TYPES[UA_TYPES_RANGE]);
    UA_Range instrumentRange = {0, 250};
    UA_Variant_setScalar(&variable_attr.value, &instrumentRange, &UA_TYPES[UA_TYPES_RANGE]);
    retval |= UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(0, 5031111), UA_NODEID_NUMERIC(0, 500111),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY), UA_QUALIFIEDNAME(0, "InstrumentRange"),
                                        UA_NODEID_NUMERIC(0, UA_NS0ID_PROPERTYTYPE), variable_attr, NULL, NULL);
    return retval;
}

int
main(void) {
    signal(SIGINT,  stopHandler);
    signal(SIGTERM, stopHandler);

    UA_StatusCode retval = UA_STATUSCODE_GOOD;
    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);

    retval |= addDataAccessNode(server);
    retval |= UA_Server_run(server, &running);

    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}

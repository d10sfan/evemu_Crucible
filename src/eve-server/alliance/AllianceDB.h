/**
 * @name AllianceDB.h
 *      database methods for alliance data
 *
 * @author Allan
 * updates James
 * @date 24 May 2019
 *
 */


#ifndef EVE_ALLIANCE_ALLIANCEDB_H
#define EVE_ALLIANCE_ALLIANCEDB_H


#include "ServiceDB.h"

#include "alliance/AllianceData.h"
#include "packets/CorporationPkts.h"

class Client;
class PyRep;
class PyObject;
class OfficeData;

class AllianceDB
{
public:

    PyRep* GetEmploymentRecord(uint32 corpID);
    bool AddEmployment(uint32 allyID, uint32 corpID);

    bool CreateAlliance(Call_CreateAlliance& allyInfo, Client* pClient, uint32& allyID, uint32& corpID);

    PyRep* GetBulletins(uint32 allyID);
    void AddBulletin(uint32 allyID, uint32 ownerID, uint32 cCharID, std::string& title, std::string& body);
    void EditBulletin(uint32 bulletinID, uint32 eCharID, int64 eDataTime, std::string& title, std::string& body);

    PyRep *GetMyApplications(uint32 allyID);
    PyRep *GetApplications(uint32 allyID);

    PyRep *GetAlliance(uint32 allyID);

    PyRep* GetContacts(uint32 allyID);
    void AddContact(uint32 allyID);
    void UpdateContact(uint32 allyID);

    PyRep* GetLabels(uint32 allyID);
    void SetLabel(uint32 allyID, uint32 color, std::string name);
    void EditLabel(uint32 allyID, uint32 labelID, uint32 color, std::string name);
    void DeleteLabel(uint32 allyID, uint32 labelID);

    bool InsertApplication(Alliance::ApplicationInfo& aInfo);
    bool UpdateApplication(const Alliance::ApplicationInfo& aInfo);
    bool DeleteApplication(const Alliance::ApplicationInfo& aInfo);
    bool GetCurrentApplicationInfo(uint32 allyID, uint32 corpID, Alliance::ApplicationInfo& aInfo);

    bool IsShortNameTaken(std::string shortName);
    bool UpdateCorpAlliance(uint32 allyID, uint32 corpID);

    PyRep* GetMembers(uint32 allyID);
    PyRep* GetAllianceMembers(uint32 allyID);
    PyRep* GetRankedAlliances();
};

#endif  // EVE_ALLIANCE_ALLIANCEDB_H
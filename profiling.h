#ifndef KIGU_PROFILING_H
#define KIGU_PROFILING_H

/*

Wrappers around Tracy's defines

to use these make sure tracy is included by you compiler and TRACY_ENABLE define is set

if you are using deshi and want the program to wait until tracy is connected define DESHI_WAIT_FOR_TRACY_CONNECTION

*/

//TODO rename these to be prefixed with KP instead

#ifdef TRACY_ENABLE
#define DPZoneNamed(varname, active)                        ZoneNamed(varname, active)                
#define DPZoneNamedN(varname, name, active)                 ZoneNamedN(varname, name, active)         
#define DPZoneNamedC(varname, color, active)                ZoneNamedC(varname, color, active)        
#define DPZoneNamedNC(varname, name, color, active)         ZoneNamedNC(varname, name, color, active) 
#define DPZoneTransient(varname, active)                    ZoneTransient(varname, active)            
#define DPZoneTransientN(varname, name, active)             ZoneTransientN(varname, name, active)     

#define DPZoneScoped                                        ZoneScoped 
#define DPZoneScopedN(name)                                 ZoneScopedN(name)
#define DPZoneScopedC(color)                                ZoneScopedC(color)
#define DPZoneScopedNC(name, color)                         ZoneScopedNC(name, color)

#define DPZoneText(txt)                                     ZoneText(txt, sizeof(txt))
#define DPZoneDynText(txt)                                  {string deshiprof_dyn_string = txt; ZoneText(deshiprof_dyn_string.str, deshiprof_dyn_string.count);}
#define DPZoneTextV(varname, txt, size)                     ZoneTextV(varname, txt, size)
#define DPZoneName(txt, size)                               ZoneName(txt, size)
#define DPZoneNameV(varname, txt, size)                     ZoneNameV(varname, txt, size)
#define DPZoneColor(color)                                  ZoneColor(color)
#define DPZoneColorV(varname, color)                        ZoneColorV(varname, color)
#define DPZoneValue(value)                                  ZoneValue(value)
#define DPZoneValueV(varname, value)                        ZoneValueV(varname, value)
#define DPZoneIsActive                                      ZoneIsActive
#define DPZoneIsActiveV(varname)                            ZoneIsActiveV(varname)

#define DPFrameMark                                         FrameMark 
#define DPFrameMarkNamed(name)                              FrameMarkNamed(name)
#define DPFrameMarkStart(name)                              FrameMarkStart(name)
#define DPFrameMarkEnd(name)                                FrameMarkEnd(name)

#define DPFrameImage(image, width, height, offset, flip)    FrameImage(image, width, height, offset, flip)

#define DPTracyLockable(type, varname)                      TracyLockable(type, varname)
#define DPTracyLockableN(type, varname, desc)               TracyLockableN(type, varname, desc)
#define DPTracySharedLockable(type, varname)                TracySharedLockable(type, varname)
#define DPTracySharedLockableN(type, varname, desc)         TracySharedLockableN(type, varname, desc)
#define DPLockableBase(type)                                LockableBase(type)
#define DPSharedLockableBase(type)                          SharedLockableBase(type)
#define DPLockMark(varname)                                 LockMark(varname)
#define DPLockableName(varname, txt, size)                  LockableName(varname, txt, size)

#define DPTracyPlot(name, val)                              TracyPlot(name, val)
#define DPTracyPlotConfig(name, type)                       TracyPlotConfig(name, type)

#define DPTracyAppInfo(txt, size)                          TracyAppInfo(txt, size)
#define DPTracyMessage(txt, size)                          TracyMessage(txt, size)   
#define DPTracyDynMessage(txt)                             {string deshi_prof_txt = txt; TracyMessage(deshi_prof_txt.str, deshi_prof_txt.count);}                           
#define DPTracyMessageL(txt)                               TracyMessageL(txt)                                
#define DPTracyMessageC(txt, size, color)                  TracyMessageC(txt, size, color)                   
#define DPTracyMessageLC(txt, color)                       TracyMessageLC(txt, color)                        

#define DPTracyAlloc(ptr, size)                            TracyAlloc(ptr, size)                             
#define DPTracyFree(ptr)                                   TracyFree(ptr)                                    
#define DPTracySecureAlloc(ptr, size)                      TracySecureAlloc(ptr, size)                       
#define DPTracySecureFree(ptr)                             TracySecureFree(ptr)                              

#define DPTracyAllocN(ptr, size, name)                     TracyAllocN(ptr, size, name)                      
#define DPTracyFreeN(ptr, name)                            TracyFreeN(ptr, name)                             
#define DPTracySecureAllocN(ptr, size, name)               TracySecureAllocN(ptr, size, name)                
#define DPTracySecureFreeN(ptr, name)                      TracySecureFreeN(ptr, name)                       

#define DPZoneNamedS(varname, depth, active)               ZoneNamedS(varname, depth, active)                
#define DPZoneNamedNS(varname, name, depth, active)        ZoneNamedNS(varname, name, depth, active)         
#define DPZoneNamedCS(varname, color, depth, active)       ZoneNamedCS(varname, color, depth, active)        
#define DPZoneNamedNCS(varname, name, color, depth, active)ZoneNamedNCS(varname, name, color, depth, active) 

#define DPZoneTransientS(varname, depth, active)           ZoneTransientS(varname, depth, active)            
#define DPZoneTransientNS(varname, name, depth, active)    ZoneTransientNS(varname, name, depth, active)     

#define DPZoneScopedS(depth)                               ZoneScopedS(depth)                                
#define DPZoneScopedNS(name, depth)                        ZoneScopedNS(name, depth)                         
#define DPZoneScopedCS(color, depth)                       ZoneScopedCS(color, depth)                        
#define DPZoneScopedNCS(name, color, depth)                ZoneScopedNCS(name, color, depth)                 

#define DPTracyAllocS(ptr, size, depth)                    TracyAllocS(ptr, size, depth)                     
#define DPTracyFreeS(ptr, depth)                           TracyFreeS(ptr, depth)                            
#define DPTracySecureAllocS(ptr, size, depth)              TracySecureAllocS(ptr, size, depth)               
#define DPTracySecureFreeS(ptr, depth)                     TracySecureFreeS(ptr, depth)                      

#define DPTracyAllocNS(ptr, size, depth, name)             TracyAllocNS(ptr, size, depth, name)              
#define DPTracyFreeNS(ptr, depth, name)                    TracyFreeNS(ptr, depth, name)                     
#define DPTracySecureAllocNS(ptr, size, depth, name)       TracySecureAllocNS(ptr, size, depth, name)        
#define DPTracySecureFreeNS(ptr, depth, name)              TracySecureFreeNS(ptr, depth, name)               

#define DPTracyMessageS(txt, size, depth)                  TracyMessageS(txt, size, depth)                   
#define DPTracyMessageLS(txt, depth)                       TracyMessageLS(txt, depth)                        
#define DPTracyMessageCS(txt, size, color, depth)          TracyMessageCS(txt, size, color, depth)           
#define DPTracyMessageLCS(txt, color, depth)               TracyMessageLCS(txt, color, depth)                
#else //DESHI_RENDERER_PROFILING
#define DPZoneNamed(varname, active)                        
#define DPZoneNamedN(varname, name, active)                 
#define DPZoneNamedC(varname, color, active)                
#define DPZoneNamedNC(varname, name, color, active)         
#define DPZoneTransient(varname, active)                    
#define DPZoneTransientN(varname, name, active)             
#define DPZoneScoped                                        
#define DPZoneScopedN(name)                                 
#define DPZoneScopedC(color)                                
#define DPZoneScopedNC(name, color)                         
#define DPZoneText(txt)                               
#define DPZoneTextV(varname, txt, size)                     
#define DPZoneName(txt, size)                               
#define DPZoneNameV(varname, txt, size)                     
#define DPZoneColor(color)                                  
#define DPZoneColorV(varname, color)                        
#define DPZoneValue(value)                                  
#define DPZoneValueV(varname, value)                        
#define DPZoneIsActive                                      
#define DPZoneIsActiveV(varname)                            
#define DPFrameMark                                         
#define DPFrameMarkNamed(name)                              
#define DPFrameMarkStart(name)                              
#define DPFrameMarkEnd(name)                                
#define DPFrameImage(image, width, height, offset, flip)    
#define DPTracyLockable(type, varname)                      
#define DPTracyLockableN(type, varname, desc)               
#define DPTracySharedLockable(type, varname)                
#define DPTracySharedLockableN(type, varname, desc)         
#define DPLockableBase(type)                                
#define DPSharedLockableBase(type)                          
#define DPLockMark(varname)                                 
#define DPLockableName(varname, txt, size)                  
#define DPTracyPlot(name, val)                              
#define DPTracyPlotConfig(name, type)                       
#define DPTracyAppInfo(txt, size)    
#define DPTracyMessage(txt, size)
#define DPTracyMessageL(txt)                               
#define DPTracyMessageC(txt, size, color)
#define DPTracyMessageLC(txt, color)

#define DPTracyAlloc(ptr, size)
#define DPTracyFree(ptr)      
#define DPTracySecureAlloc(ptr, size)
#define DPTracySecureFree(ptr)

#define DPTracyAllocN(ptr, size, name)
#define DPTracyFreeN(ptr, name)
#define DPTracySecureAllocN(ptr, size, name)
#define DPTracySecureFreeN(ptr, name)

#define DPZoneNamedS(varname, depth, active)
#define DPZoneNamedNS(varname, name, depth, active)
#define DPZoneNamedCS(varname, color, depth, active)
#define DPZoneNamedNCS(varname, name, color, depth, active)

#define DPZoneTransientS(varname, depth, active)
#define DPZoneTransientNS(varname, name, depth, active)

#define DPZoneScopedS(depth)                               
#define DPZoneScopedNS(name, depth)
#define DPZoneScopedCS(color, depth)
#define DPZoneScopedNCS(name, color, depth)

#define DPTracyAllocS(ptr, size, depth)
#define DPTracyFreeS(ptr, depth)
#define DPTracySecureAllocS(ptr, size, depth)
#define DPTracySecureFreeS(ptr, depth)

#define DPTracyAllocNS(ptr, size, depth, name)
#define DPTracyFreeNS(ptr, depth, name)
#define DPTracySecureAllocNS(ptr, size, depth, name)
#define DPTracySecureFreeNS(ptr, depth, name)

#define DPTracyMessageS(txt, size, depth)
#define DPTracyMessageLS(txt, depth)
#define DPTracyMessageCS(txt, size, color, depth)
#define DPTracyMessageLCS(txt, color, depth)
#endif


//#include "hash.h"
//#include "map.h"
//#include "array.h"
//
//
//struct FunctionCall{
//    cstring name;
//    TNode node; //TNode that points to other function calls 
//    u32 hash;
//    b32 ended = 0;
//    ~FunctionCall(){
//        Assert(ended, "Forgot to call KPFuncEnd for a KPFuncBegin or the scope KPFuncStart was called in ended earlier than expected");
//    }
//};

#ifdef KIGU_ENABLE_PROFILER
//maybe do an atom approach where we just make a static varaible that is a handle to an element of a map
//since this should only be called once per function it should work fine 
#define KPFuncStart\
 FunctionCall ___Func___KiguProfilerVariableDONOTTOUCHplease; \
 ___Func___KiguProfilerVariableDONOTTOUCHplease.hash = hash<char*>()(toStr(__func__, __LINE__, g_profiler->stackcount).str); \
 g_profiler->funccall(__func__, ___Func___KiguProfilerVariableDONOTTOUCHplease.hash)

#define KPFuncEnd\
 ___Func___KiguProfilerVariableDONOTTOUCHplease.ended = true; \
 g_profiler->funcend(___Func___KiguProfilerVariableDONOTTOUCHplease.hash);
#else

#define KPFuncStart
#define KPFuncEnd

#endif

//struct Profiler{
//    TNode* currfunc;
//    map<u32, FunctionCall> knownFuncs;
//    u64 stackcount = 0; //for differenciating between a function being called at different stack levels
//    
//    void funccall(const char* name, u32 hash) {
//        stackcount++;
//        if(!knownFuncs.has(hash)) knownFuncs.add(hash);
//        FunctionCall* fc = knownFuncs.at(hash);
//        fc->name={(char*)name,strlen(name)}; //make this not happen everytime or something
//        if(currfunc){ 
//            //check if current function has already called this function
//            b32 exists=false;
//            for_node(currfunc->first_child) if(it==&fc->node) { exists=true; break; }
//            if(!exists) insert_last(currfunc, &fc->node);
//        }
//        currfunc = &fc->node;
//    }
//
//    //enforce passing hash so we make sure that the function we are ending is the same as current
//    //TODO setup checking that the function is the same eventually
//    void funcend(u32 hash){
//        stackcount--;
//        if(!knownFuncs.has(hash)) Assert(0, "attempted to use funcend with a hash that is unknown");
//        if(currfunc)
//            currfunc = currfunc->parent;
//    }
//};
//
//extern Profiler* g_profiler;
//#define KiguProfiler g_profiler
//#define KiguDefineProfiler local Profiler __kigu_profiler__; Profiler* g_profiler = &__kigu_profiler__;

#endif //KIGU_PROFILING_H
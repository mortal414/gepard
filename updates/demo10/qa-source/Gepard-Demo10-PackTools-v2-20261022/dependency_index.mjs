// Index/config metadata only. Never extract/read dependency resource bodies.
import fs from 'node:fs';import path from 'node:path';import crypto from 'node:crypto';import {fileURLToPath} from 'node:url';
import {readPakIndex} from 'file:///C:/Users/marco/Desktop/reforger-agent-harness/dist/src/pak-source.js';
const here=path.dirname(fileURLToPath(import.meta.url));
const root='D:\\ArmaProjects\\Gepard-Demo10-TrackArt-20261022\\private-demo-source10\\dependencies';
const sha=p=>crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex');
const results=[];
for(const folder of ['WCS_Armaments_629B2BA37EFFD577','SpaceCore_5E389BB9F58B79A6']){
 const project=path.join(root,folder,'addon.gproj'),pak=path.join(root,folder,'data.pak'),text=fs.readFileSync(project,'utf8');
 const source=/StringTableSource\s+"\{([A-F0-9]+)\}([^"]+)"/.exec(text);
 const runtime=[...text.matchAll(/"\{([A-F0-9]+)\}(Language\/[^"\r\n]+\.conf)"/g)];
 if(!source||runtime.length!==13)throw Error('Exact source/13-runtime localization declaration expected');
 const ix=await readPakIndex(pak),entries=ix.entries;
 const find=n=>entries.filter(e=>e.resource.toLowerCase()===n.toLowerCase());
 const rows=runtime.map(m=>{const e=find(m[2]);if(e.length!==1||e[0].rawBytes<=0||e[0].storedBytes<=0)throw Error('Missing/empty/ambiguous declared runtime '+m[2]);return {guid:m[1],resource:m[2],...e[0]};});
 results.push({folder,project,projectSHA256:sha(project),pak,pakSHA256:sha(pak),pakIndexSHA256:ix.indexSha256,indexEntries:entries.length,source:{guid:source[1],resource:source[2],matchingIndexEntries:find(source[2]).length},runtime:rows,all13DeclaredRuntimeResourcesPresentNonempty:true,resourceBodiesRead:false});
}
const report={schema:1,status:'EXACT_EDITOR_SOURCES_ABSENT_26_DECLARED_RUNTIME_CONFS_INDEX_PRESENT',results,scope:'Index existence/size plus project declarations only; no runtime localization/rendered text validation; no warning suppression or source edits',DWritten:false,nativeStarted:false};
fs.writeFileSync(path.join(here,'DEPENDENCY-LOCALIZATION-INDEX.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));

/* Pure measurement utilities shared by the UI and regression checks. */
(function(root){
'use strict';
const fields=[['power','Active power','W',1,'bolt'],['voltage','Voltage','V',1,'pulse'],['current','Current','A',3,'arrow'],['energy','Meter energy','kWh',4,'bolt'],['frequency','Frequency','Hz',2,'pulse'],['powerFactor','Power factor','',2,'arrow']];
const resolution={power:.1,voltage:.1,current:.001,energy:.001,frequency:.1,powerFactor:.01};
function clean(raw,now=Date.now()){
 const r={timestamp:new Date(now).toISOString(),receivedAt:new Date(now).toISOString()},quality=[];
 if(raw.timestamp){const t=Date.parse(raw.timestamp);if(Number.isFinite(t)&&Math.abs(t-now)<86400000)r.timestamp=new Date(t).toISOString();else quality.push('invalid_or_unsynchronized_timestamp')}
 for(const[k]of fields){const x=raw[k];r[k]=typeof x==='number'&&Number.isFinite(x)&&x>=0&&(k!=='powerFactor'||x<=1)?x:null;if(r[k]===null)quality.push('missing_or_invalid_'+k)}
 if(raw.sensorOk===false){quality.push('sensor_read_failed');for(const[k]of fields)r[k]=null}
 for(const k of ['sensorOk','error','consecutiveErrors','sequence','uptimeMs'])if(Object.hasOwn(raw,k))r[k]=raw[k];
 r.quality=quality;return r;
}
function stats(rs){let sum=0,count=0,peak=null,wh=0,validSeconds=0;
 for(let i=0;i<rs.length;i++){const b=rs[i];if(Number.isFinite(b.power)){sum+=b.power;count++;peak=peak===null?b.power:Math.max(peak,b.power)}if(!i)continue;const a=rs[i-1],dt=(Date.parse(b.timestamp)-Date.parse(a.timestamp))/1000;if(dt>0&&dt<=5&&Number.isFinite(a.power)&&Number.isFinite(b.power)){wh+=(a.power+b.power)/2*dt/3600;validSeconds+=dt}}
 return{avg:count?sum/count:null,peak,wh,validSeconds};
}
function scale(values,key,mode='zero',zoom=1){const v=values.filter(Number.isFinite),lo=v.length?Math.min(...v):0,hi=v.length?Math.max(...v):1;const minSpan=resolution[key]*2;
 if(mode==='zero')return {min:0,max:Math.max(hi*1.12,minSpan),clipped:0};
 const base=Math.max((hi-lo)*1.25,minSpan),span=Math.max(base/zoom,minSpan/10),center=(hi+lo)/2,min=Math.max(0,center-span/2),max=min+span;
 return{min,max,clipped:v.filter(x=>x<min||x>max).length};
}
function validateSession(s){
 if(!s||s.schemaVersion!==1||!['demo','serial'].includes(s.source)||typeof s.appliance!=='string'||s.appliance.length>200||!Array.isArray(s.readings)||s.readings.length>14400||!Number.isFinite(Date.parse(s.startedAt))||!Number.isFinite(Date.parse(s.endedAt))||Date.parse(s.endedAt)<Date.parse(s.startedAt))throw Error('This is not a supported Pulse session.');
 const readings=s.readings.map(r=>{if(!r||!Number.isFinite(Date.parse(r.timestamp)))throw Error('A reading has an invalid timestamp.');const n=clean(r,Date.parse(r.timestamp));n.timestamp=new Date(r.timestamp).toISOString();n.receivedAt=Number.isFinite(Date.parse(r.receivedAt))?new Date(r.receivedAt).toISOString():n.timestamp;n.source=s.source;return n});
 const notes=Array.isArray(s.notes)?s.notes.filter(n=>n&&typeof n.text==='string'&&Number.isFinite(Date.parse(n.timestamp))).slice(0,14400).map(n=>({text:n.text.slice(0,500),timestamp:new Date(n.timestamp).toISOString()})):[];
 return{schemaVersion:1,id:typeof s.id==='string'?s.id:'',appliance:s.appliance,source:s.source,startedAt:new Date(s.startedAt).toISOString(),endedAt:new Date(s.endedAt).toISOString(),readings,notes,analysis:Array.isArray(s.analysis)?s.analysis:[]};
}
root.PulseCore={fields,resolution,clean,stats,scale,validateSession};if(typeof module!=='undefined')module.exports=root.PulseCore;
})(globalThis);

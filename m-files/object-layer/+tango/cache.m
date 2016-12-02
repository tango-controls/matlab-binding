function dev = cache(devname)

global MACHDEVCACHE

if isempty(MACHDEVCACHE)
    MACHDEVCACHE=containers.Map('KeyType','char','ValueType','any');
end
if nargin >= 1
    try
        dev=MACHDEVCACHE(devname);
    catch
        dev=tango.Device(devname);
        MACHDEVCACHE(devname)=dev;
    end
else
    MACHDEVCACHE.remove(MACHDEVCACHE.keys);
end
end

function vers = version()
%version returns the Tango version and the Tango matlab-binding version

vers=struct('TangoVersion',tango_version,'TangoBindingVersion',tango_mex_version);
end


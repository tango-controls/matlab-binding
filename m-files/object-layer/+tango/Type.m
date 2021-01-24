classdef Type < uint8
    %TangoType Enumeration representing Tango data types
    
    methods(Static,Access=private,Hidden)
        function uval=undef()
            uval=cell(1,30);
            uval(1:9)={[],false,int16(Inf),int32(Inf),single(NaN),NaN,...
                uint16(Inf),uint32(Inf),''};
            uval{17}={};
            uval(20:21)={tango.DevState.Unknown,''};
            uval(23:25)={uint8(NaN),int64(Inf),uint64(Inf)};
        end
    end
    methods
        function t=isMatrixCompatible(self)
            persistent tab
            if isempty(tab)
                tab=false(30);
                tab([2:8 20 23:25 28 29])=true;
            end
            t=tab(self+1);
        end
        function val=Undefined(self)
            persistent uval
            if isempty(uval)
                uval=self.undef();
            end
            val=uval{self+1};
        end
    end
    enumeration
        DEV_VOID            (0)
        DEV_BOOLEAN         (1)
        DEV_SHORT           (2)
        DEV_LONG            (3)
        DEV_FLOAT           (4)
        DEV_DOUBLE          (5)
        DEV_USHORT          (6)
        DEV_ULONG           (7)
        DEV_STRING          (8)
        DEVVAR_CHARARRAY	(9)
        DEVVAR_SHORTARRAY	(10)
        DEVVAR_LONGARRAY	(11)
        DEVVAR_FLOATARRAY	(12)
        DEVVAR_DOUBLEARRAY	(13)
        DEVVAR_USHORTARRAY	(14)
        DEVVAR_ULONGARRAY	(15)
        DEVVAR_STRINGARRAY	(16)
        DEVVAR_LONGSTRINGARRAY	(17)
        DEVVAR_DOUBLESTRINGARRAY(18)
        DEV_STATE           (19)
        CONST_DEV_STRING	(20)
        DEVVAR_BOOLEANARRAY	(21)
        DEV_UCHAR           (22)
        DEV_LONG64          (23)
        DEV_ULONG64         (24)
        DEVVAR_LONG64ARRAY	(25)
        DEVVAR_ULONG64ARRAY	(26)
        DEV_INT             (27)
        DEV_ENCODED         (28)
        DEV_ENUM            (29)
    end
    
end


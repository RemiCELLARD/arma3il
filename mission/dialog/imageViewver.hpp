
class A3IL_ImageViewer {
    idd = 26214;
    movingEnable = 0;
    enableSimulation = 1;

    class controlsBackground {
        class Background: A3IL_RscText {
            idc = -1;
            x = safezoneX;
            y = safezoneY; 
            w = safezoneW;
            h = safezoneH;
            colorBackground[] = {0, 0, 0, 0.75}; // noir semi-transparent
        };
    };

    class controls {
        class Picture: A3IL_RscPicture {
            idc = 26215;
            text = "";
            x = 0;
            y = 0;
            w = 0.1;
            h = 0.1;
        };

        class Title: A3IL_RscStructuredText {
            idc = 26220;
            text = "$STR_A3IL_Global_Loading";
            x = 0;
            y = 0;
            w = 0.1;
            h = 0.04;
            style = 0x02; // ST_CENTER
            sizeEx = (((((safezoneW / safezoneH) min 1.2) / 1.2) / 25) * 1.15);
            colorText[] = {1, 1, 1, 1};
            colorBackground[] = {0, 0, 0, 0};
            shadow = 0;
            class Attributes {
                font = "PuristaMedium";
                color = "#ffffff";
                align = "center";
                valign = "middle";
                shadow = 0;
            };
        };

        class CloseButton: A3IL_RscButtonMenu {
            idc = -1;
            text = "X";
            tooltip = "$STR_A3IL_Global_Close";
            style = 0x02 + 0xC0;
            x = ((safezoneX + safezoneW) - 0.08);
            y = (safezoneY + 0.015);
            w = 0.065;
            h = 0.065;
            action = "closeDialog 0;";

            class TextPos
            {
                left = 0;
                top = 0;
                right = 0;
                bottom = 0;
                forceMiddle = 1;
            };
            class Attributes
            {
                font = "RobotoCondensed";
                color = "#E5E5E5";
                align = "center";
                valign = "middle";
                shadow = 1;
                size = 1;
            };
        };
    };
};
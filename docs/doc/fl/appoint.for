	STRUCTURE /DATE/
	    INTEGER*1 DAY
	    INTEGER*1 MONTH
*	    INTEGER*2 YEAR /1991/
	    INTEGER*2 YEAR
	END STRUCTURE

	STRUCTURE /TIME/
	    INTEGER*1	 HOUR, MINUTE
	END STRUCTURE

	STRUCTURE /APPOINTMENT/
	    RECORD /DATE/   APP_DATE
*	    STRUCTURE /TIME/ APP_TIME(2)
*		INTEGER*1    HOUR, MINUTE
*	    END STRUCTURE
	    RECORD /TIME/   APP_TIME(2)
	    CHARACTER*20    APP_MEMO(4)
	    INTEGER*1	    APP_FLAG
	END STRUCTURE

	RECORD /APPOINTMENT/ ITEM_1, ITEM_2

	ITEM_1.APP_DATE.YEAR = 1991
	ITEM_1.APP_DATE.MONTH = 05
	ITEM_1.APP_DATE.DAY = 04
	ITEM_1.APP_TIME(1).HOUR = 12
	ITEM_1.APP_TIME(1).MINUTE = 10
	ITEM_1.APP_TIME(2).HOUR = 13
	ITEM_1.APP_TIME(2).MINUTE = 15
	ITEM_1.APP_MEMO(1) = 'This is line 1'
	ITEM_1.APP_MEMO(2) = 'This is line 2'
	ITEM_1.APP_MEMO(3) = 'This is line 3'
	ITEM_1.APP_MEMO(4) = 'This is line 4'
	ITEM_1.APP_FLAG = 127

	PRINT *, ITEM_1
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(1)
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(2)
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(1).HOUR
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(1).MINUTE
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(2).HOUR
	PRINT *, '----------------------------------'
	PRINT *, ITEM_1.APP_TIME(2).MINUTE
	PRINT *, '----------------------------------'
	END

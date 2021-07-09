class ShipInterfaceClass
{
	static var s_shipInterface:ShipInterfaceClass;
	static var s_testLRValue:Number = 0;
	static var s_testUDValue:Number = 0;

    public static function main(mc:MovieClip):Void
    {
		trace("Ship interface class initialization");
        s_shipInterface = new ShipInterfaceClass();
    }

	public function onKeyDown(event:Key):Void
	{
		switch(Key.getCode())
		{
		case 37: // LEFT ARROW ←
		case 65: // A
		    s_testLRValue -= 0.9;
			_root.SetTestLRValue(s_testLRValue);
			break;
		case 39: // RIGHT ARROW →
		case 68: // D
		    s_testLRValue += 0.9;
			_root.SetTestLRValue(s_testLRValue);
			break;
		case 40: // DOWN ARROW ↓
		case 87: // W
		    s_testUDValue += 0.9;
			_root.SetTestUDValue(s_testUDValue);
			break;
		case 38: // UP ARROW ↑
		case 83: // W
		    s_testUDValue -= 0.9;
			_root.SetTestUDValue(s_testUDValue);
			break;
		}
	}
	
	public function ShipInterfaceClass() 
	{
		if (_root.g_isTesting)
		{
			Key.addListener(this);
		}
	}
}
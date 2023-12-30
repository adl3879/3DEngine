using Coral.Managed.Interop;

namespace ScriptCore.Net
{
	/// <summary>
	/// This is the core ScriptCore.Net API. 
	/// All internal call should happen in this file.
	/// </summary>

	public class Engine
	{
		internal static unsafe delegate*<NativeString, void> LoggerLogIcall;

		public static void Test()
		{
			System.Console.WriteLine("Hello World!");
		}

		/// <summary>
		/// Prints a message to the console log
		/// </summary>
		/// <param name="message">message to be printed</param>
		public static void Log()
		{
			NativeString str = "What is going on here?";
			unsafe { LoggerLogIcall(str); }
		}
	}
}

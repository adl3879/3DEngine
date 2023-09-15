using Engine;
using System;

namespace Sandbox
{
	public class Player : Entity
	{
		public float Speed;
		public bool IsAlive;

		void OnCreate()
		{
			Console.WriteLine("Player created!");
		}

		void OnUpdate(float dt)
		{
			// Console.WriteLine($"Player updated! {dt}");
			Console.WriteLine($"Speed {Speed} {dt}");
			if (Input.IsKeyDown(KeyCode.W))
			{
				Console.WriteLine("W is down!");
			}
		}
	}
}

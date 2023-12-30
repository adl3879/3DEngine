using ScriptCore.Net;
using System.Numerics;

namespace SandboxProject
{
	class Player : Entity
	{
		public float Speed = 5.0f;
		public int Flight;

		private TransformComponent transform;

		public override void OnInit()
		{
			// Called once at the start of the game
			System.Console.WriteLine($"Player created");

			transform = GetComponent<TransformComponent>();
		}

		public override void OnUpdate(float dt)
		{
			var horizontal = Input.GetAxis("Horizontal");
			var vertical = Input.GetAxis("Vertical");

			transform.Translate(new Vector3(horizontal, vertical, 0) * Speed * dt);

			// rotate the player
			var mouseX = Input.GetAxis("Mouse X");
			//var mouseY = Input.GetAxis("Mouse Y");

			//transform.Rotate(new Vector3(0, 0, -mouseX * 100 * dt));
			if (Input.IsActionPressed("jump"))
			{
				System.Console.WriteLine("jumping");
			}
		}

		public override void OnFixedUpdate(float dt)
		{
			// Called every fixed update
			// System.Console.WriteLine($"delta time is: {dt * Speed}");
		}

		public override void OnDestroy()
		{
			// Called at the end of the game fixed update
			System.Console.WriteLine("Player destroyed");
		}
	}
}

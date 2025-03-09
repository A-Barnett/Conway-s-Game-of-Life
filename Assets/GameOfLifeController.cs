using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameOfLifeController : MonoBehaviour
{
    public ComputeShader computeShader;
    public ComputeShader toggleShader;
    public int width = 64;
    public int height = 64;
    public float simulationSpeed = 1.0f;

    [SerializeField] private RenderTexture currentState;
    [SerializeField] private RenderTexture nextState;
    [SerializeField] private GameObject quad;
    private float timer;
    private bool running = false;
    void Start()
    {
        InitializeTextures();
        InitializeGrid();
        RunSimulation();
    }

    void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            ToggleCellState();
        }

        if (Input.GetKeyDown(KeyCode.Q))
        {
            running = !running;
            SetQuadTexture();
            Debug.Log("Running: " + running);
        }

        timer += Time.deltaTime;
        if (timer >= simulationSpeed)
        {
            timer = 0.0f;
            if (running)
            { 
                RunSimulation();
            }
        }
    }

    void InitializeTextures()
    {
        //currentState = new RenderTexture(width, height, 0, RenderTextureFormat.RFloat);
        //currentState.enableRandomWrite = true;
        currentState.Create();

        //nextState = new RenderTexture(width, height, 0, RenderTextureFormat.RFloat);
        //nextState.enableRandomWrite = true;
        nextState.Create();
    }

    void InitializeGrid()
    {
        RenderTexture.active = currentState;
        GL.Clear(false, true, Color.black);
        RenderTexture.active = null;
    }

    void ToggleCellState()
    {
        // Get the mouse position in screen coordinates
        Vector2 mousePos = Input.mousePosition;

        // Convert the screen position of the mouse to a Ray
        Ray ray = Camera.main.ScreenPointToRay(mousePos);
        RaycastHit hit;

        // Perform a raycast to check if the quad is hit
        if (Physics.Raycast(ray, out hit))
        {
            // Get the local texture coordinates
            Vector2 uv = hit.textureCoord;

            // Convert the local texture coordinates to pixel coordinates
            int x = Mathf.FloorToInt(uv.x * width);
            int y = Mathf.FloorToInt(uv.y * height);

            // Ensure the coordinates are within the texture bounds
            if (x >= 0 && x < width && y >= 0 && y < height)
            {
                // Set up the compute shader to toggle the cell state
                int kernelHandle = toggleShader.FindKernel("ToggleCell");
                toggleShader.SetTexture(kernelHandle, "currentState", currentState);
                toggleShader.SetInts("cellPos", x, y);
                toggleShader.Dispatch(kernelHandle, 1, 1, 1);
            }
        }
    }

    void RunSimulation()
    {
        int kernelHandle = computeShader.FindKernel("CSMain");
        computeShader.SetTexture(kernelHandle, "currentState", currentState);
        computeShader.SetTexture(kernelHandle, "nextState", nextState);
        computeShader.Dispatch(kernelHandle, width / 8, height / 8, 1);
   

        // Swap the textures
        RenderTexture temp = currentState;
        currentState = nextState;
        nextState = temp;
    }
    void SetQuadTexture()
    {
        Material quadMaterial = quad.GetComponent<Renderer>().material;
        quadMaterial.mainTexture = currentState;
    }

}
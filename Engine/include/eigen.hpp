#include <glm/glm.hpp>

/*
Functions here were taken from https://github.com/berkerdemirel/Linear-Algebra
Just necessary functions were added to avoid include the whole repository.
Functions were modified to support glm types.
*/

void print_matrix(glm::mat3 M){
    for (int a=0; a<3; a++) {
        std::cout << "| ";
        for (int b=0; b<3; b++){
            std::cout << M[a][b] << "\t";
        } 
        std::cout << "|";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

glm::mat3 elementWiseSquare(glm::mat3 matrix)
{
    glm::mat3 result;
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            result[i][j] = matrix[i][j] * matrix[i][j];
        }
    }

    return result;
}

glm::vec3 sum(glm::mat3 matrix, int axis)
{
    glm::vec3 result(0.0f);

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            if (axis==0)
            {
                result[i] += matrix[i][j];
            }
            else if(axis==1)
            {
                result[i] += matrix[j][i];
            }
            else
            {
                result[0] += matrix[i][j];
            }
        }
    }

    return result;
}

glm::vec3 getColumn(glm::mat3 matrix, int col)
{
    return glm::vec3(matrix[0][col], matrix[1][col], matrix[2][col]);
}

glm::vec3 getRow(glm::mat3 matrix, int row)
{
    return glm::vec3(matrix[row][0], matrix[row][1], matrix[row][2]);
}

glm::mat3 concatenateColumns(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::mat3 result;

    for (int i=0; i<3; i++)
    {
        result[i][0] = a[i];
        result[i][1] = b[i];
        result[i][2] = c[i];
    }

    return result;
}

void overwriteColumn(glm::mat3& matrix, glm::vec3 vector, int col)
{
    for (int i=0; i<3; i++)
    {
        matrix[i][col] = vector[i];
    }
}

double frobeniusNorm(glm::mat3 matrix)
{
    return sqrt(sum(elementWiseSquare(matrix), -1)[0]);
}

std::tuple<glm::mat3, double> jacobiRotationMatrix(double alpha, double beta, double gamma)
{
    glm::mat3 G(0.0f);

    double c = 1.0f;
    double s = 0.0f;
    double t = 0.0f;

    if (beta != 0) {
		double tau = (gamma - alpha) / (2 * beta);
		if (tau >= 0)
        {
			t = 1 / (tau + sqrt(1 + tau * tau));
		}
		else
        {
			t = -1 / (-tau + sqrt(1 + tau * tau));
		}
		c = 1 / sqrt(1 + t * t);
		s = t*c;
	}

    G[0][0] =  c;
    G[0][1] =  s;
    G[1][0] = -s;
    G[1][1] =  c;
    s = t;

    return {G, t};
}

glm::vec3 getEigenvaluesFromMatrix(glm::mat3 matrix)
{
    glm::vec3 sigma = sum(elementWiseSquare(matrix), 1);
    double eps = 1e-15;
    double rots = 1.0;
    double tolsigma = eps*frobeniusNorm(matrix);

    // iterate until all (p,q) pairs give less error than tolerance
    while (rots >= 1.0)
    {
        rots = 0.0;
        for (int p=0; p<2; p++)
        {
            for (int q=p+1; q<3; q++)
            {
                glm::vec3 a = getColumn(matrix, p);
                glm::vec3 b = getColumn(matrix, q);
                double beta = a.x*b.x + a.y*b.y + a.z*b.z;
                if ((sigma[p]*sigma[q] > tolsigma) && (abs(beta) >= eps*sqrt(sigma[p]*sigma[q])))
                {
                    rots++;
                    auto [G, t] = jacobiRotationMatrix(sigma[p], beta, sigma[q]);
                    // update eigenvalues
                    sigma[p] = sigma[p] - beta*t;
                    sigma[q] = sigma[q] + beta*t;
                    glm::mat3 temp = G * concatenateColumns(a, b, glm::vec3(0.0f));
                    // update pth and qth cols of the matrix
                    overwriteColumn(matrix, getColumn(temp, 0), p);
                    overwriteColumn(matrix, getColumn(temp, 1), q);
                }
            }
        }

        break;
    }

    return sigma;
}
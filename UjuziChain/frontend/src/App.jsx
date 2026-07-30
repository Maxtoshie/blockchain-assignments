import { Routes, Route } from "react-router-dom";
import LandingPage from "./pages/LandingPage";
import AboutPage from "./pages/AboutPage";
import EmployerVerifyPage from "./pages/EmployerVerifyPage";
import CredentialDetailsPage from "./pages/CredentialDetailsPage";
import AdminDashboard from "./pages/AdminDashboard";
import InstitutionDashboard from "./pages/InstitutionDashboard";
import StudentDashboard from "./pages/StudentDashboard";

export default function App() {
  return (
    <Routes>
      <Route path="/" element={<LandingPage />} />
      <Route path="/about" element={<AboutPage />} />
      <Route path="/verify" element={<EmployerVerifyPage />} />
      <Route path="/credentials/:id" element={<CredentialDetailsPage />} />
      <Route path="/dashboard/admin" element={<AdminDashboard />} />
      <Route path="/dashboard/institution" element={<InstitutionDashboard />} />
      <Route path="/dashboard/student" element={<StudentDashboard />} />
    </Routes>
  );
}
